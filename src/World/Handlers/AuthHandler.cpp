/*
 * Copyright (C) 2015 Dehravor <dehravor@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "WorldSession.h"
#include "Cryptography/SHA1.h"
#include "Config.h"
#include "Addon.h"
#include <zlib/zlib.h>

void WorldSession::HandleConnectionVerification(WorldPacket &recvPacket)
{
    std::string serverString;
    recvPacket >> serverString;

    if (serverString != "RLD OF WARCRAFT CONNECTION - SERVER TO CLIENT")
    {
        print("Connection verification failed. Invalid string received: %s", serverString.c_str());
        return;
    }

    std::string clientString = "D OF WARCRAFT CONNECTION - CLIENT TO SERVER";

    WorldPacket response(MSG_VERIFY_CONNECTIVITY, clientString.length() + 1);
    response << clientString;
    SendPacket(response);
}

void WorldSession::HandleAuthChallenge(WorldPacket &recvPacket)
{
    // Read server values
    uint32 keys[8];

    for (int i = 0; i < 8; i++)
        recvPacket >> keys[i];

    recvPacket >> serverSeed_;

    uint8 unk;
    recvPacket >> unk;

    // Generate our proof
    uint32 zero = 0;

    SHA1 proof;
    proof.Update(session_->GetAccountName());
    proof.Update((uint8*)&zero, sizeof(uint32));
    proof.Update((uint8*)&clientSeed_, sizeof(uint32));
    proof.Update((uint8*)&serverSeed_, sizeof(uint32));
    proof.Update(session_->GetKey());
    proof.Finalize();

    uint8* digest = proof.GetDigest();
 
    WorldPacket response(CMSG_AUTH_SESSION);
    response << uint32(0); // 00 00 00 00 [4.3.4 15595 enUS]
    response << uint32(0); // 00 00 00 00 [4.3.4 15595 enUS]
    response << uint8(0); // 00 [4.3.4 15595 enUS]
    response << digest[10];
    response << digest[18];
    response << digest[12];
    response << digest[5];
    response << uint64(3); // 03 00 00 00 00 00 00 00 [4.3.4 15595 enUS]
    response << digest[15];
    response << digest[9];
    response << digest[19];
    response << digest[4];
    response << digest[7];
    response << digest[16];
    response << digest[3];
    response << uint16(GameBuild);
    response << digest[8];
    response << uint32(1); // 01 00 00 00  [4.3.4 15595 enUS]
    response << uint8(1);
    response << digest[17];
    response << digest[6];
    response << digest[0];
    response << digest[1];
    response << digest[11];
    response << uint32(clientSeed_);
    response << digest[2];
    response << uint32(0); // 00 00 00 00 [4.3.4 15595 enUS]
    response << digest[14];
    response << digest[13];

    ByteBuffer addonData;
    addonData << uint32(AddonDatabase.size());

    for (auto const& addon : AddonDatabase)
    {
        addonData << addon.Name;
        addonData << uint8(addon.Enabled);
        addonData << uint32(addon.CRC);
        addonData << uint32(addon.Unknown);
    }

    addonData << uint32(0x4B44A47C); // 7C A4 44 4B [4.3.4 15595 enUS]

    uLongf compressedSize = compressBound(addonData.size());

    ByteBuffer addonDataCompressed;
    addonDataCompressed.resize(compressedSize);

    if (compress(addonDataCompressed.contents(), &compressedSize, addonData.contents(), addonData.size()) != Z_OK)
        assert(false);

    response << uint32(4 + compressedSize);
    response << uint32(addonData.size());
    response.append(addonDataCompressed.contents(), compressedSize);

    response.WriteBit(0);
    response.WriteBits(session_->GetAccountName().length(), 12);
    response.FlushBits();
    response.WriteString(session_->GetAccountName());

    SendPacket(response);
}

enum AuthResult : uint8
{
    AUTH_OK                     = 12,
    AUTH_FAILED                 = 13,
    AUTH_REJECT                 = 14,
    AUTH_BAD_SERVER_PROOF       = 15,
    AUTH_UNAVAILABLE            = 16,
    AUTH_SYSTEM_ERROR           = 17,
    AUTH_BILLING_ERROR          = 18,
    AUTH_BILLING_EXPIRED        = 19,
    AUTH_VERSION_MISMATCH       = 20,
    AUTH_UNKNOWN_ACCOUNT        = 21,
    AUTH_INCORRECT_PASSWORD     = 22,
    AUTH_SESSION_EXPIRED        = 23,
    AUTH_SERVER_SHUTTING_DOWN   = 24,
    AUTH_ALREADY_LOGGING_IN     = 25,
    AUTH_LOGIN_SERVER_NOT_FOUND = 26,
    AUTH_WAIT_QUEUE             = 27,
    AUTH_BANNED                 = 28,
    AUTH_ALREADY_ONLINE         = 29,
    AUTH_NO_TIME                = 30,
    AUTH_DB_BUSY                = 31,
    AUTH_SUSPENDED              = 32,
    AUTH_PARENTAL_CONTROL       = 33,
    AUTH_LOCKED_ENFORCED        = 34,
};

void WorldSession::HandleAuthResponse(WorldPacket &recvPacket)
{
    uint32 billingTimeRemaining, billingTimeRested, queuePosition;
    uint8 billingPlanFlags, result, expansion;
    bool hasQueueInfo, hasAccountInfo;

    hasQueueInfo = recvPacket.ReadBit();

    if (hasQueueInfo)
        recvPacket.ReadBit();

    hasAccountInfo = recvPacket.ReadBit();

    if (hasAccountInfo)
    {
        recvPacket >> billingTimeRemaining;
        recvPacket >> billingPlanFlags;
        recvPacket >> billingTimeRested;
        recvPacket >> expansion;
    }

    recvPacket >> result;

    if (hasQueueInfo && result == AUTH_OK)
    {
        recvPacket >> queuePosition;
        print("%s is full. Position in queue: %d", session_->GetRealmName().c_str(), queuePosition);
    }

    if (!hasAccountInfo)
        return;

    print("%s", "[World]");
    print("%s", "Successfully authenticated!");

    WorldPacket packet(CMSG_REALM_SPLIT, 4);
    packet << uint32(0xFFFFFFFF);
    SendPacket(packet);

    packet.Initialize(CMSG_CHAR_ENUM, 0);
    SendPacket(packet);
}

void WorldSession::HandleRedirect(WorldPacket &recvPacket)
{
    assert(false);
}