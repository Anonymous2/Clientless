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

void WorldSession::HandleAuthenticationChallenge(WorldPacket &recvPacket)
{
    uint32 unk;
    uint32 serverSeed;

    recvPacket >> unk;
    recvPacket >> serverSeed;

    uint8 encryptionSeed[2][16];

    recvPacket.read(encryptionSeed[0], 16);
    recvPacket.read(encryptionSeed[1], 16);

    uint32 zero = 0;
    uint32 clientSeed = 0x4B8C87D0;

    SHA1 authResponse;
    authResponse.Update(session_->GetAccountName());
    authResponse.Update((uint8*)&zero, sizeof(uint32));
    authResponse.Update((uint8*)&clientSeed, sizeof(uint32));
    authResponse.Update((uint8*)&serverSeed, sizeof(uint32));
    authResponse.Update(session_->GetKey());
    authResponse.Finalize();

    WorldPacket response(CMSG_AUTH_SESSION);
    response << uint32(GameBuild);
    response << uint32(0);
    response << session_->GetAccountName();
    response << uint32(0);
    response << clientSeed;
    response << uint32(0);
    response << uint32(0);
    response << uint32(session_->GetRealm().ID);
    response << uint64(0);
    response.append(authResponse.GetDigest(), 20);

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

void WorldSession::HandleAuthenticationResponse(WorldPacket &recvPacket)
{
    uint8 result, billingFlags, expansion;
    uint32 billingTimeRemaining, billingTimeRested;

    recvPacket >> result;

    if (result == AUTH_WAIT_QUEUE)
    {
        uint32 queuePosition;
        recvPacket >> queuePosition;

        if (queuePosition > 0)
            print("%s is full. Position in queue: %d", session_->GetRealmName().c_str(), queuePosition);

        return;
    }

    recvPacket >> billingTimeRemaining;
    recvPacket >> billingFlags;
    recvPacket >> billingTimeRested;
    recvPacket >> expansion;

    print("%s", "[World]");
    print("%s", "Successfully authenticated!");

    WorldPacket packet(CMSG_CHAR_ENUM, 0);
    SendPacket(packet);
}