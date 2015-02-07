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

void WorldSession::HandleAuthenticationChallenge(WorldPacket &recvPacket)
{
    recvPacket >> serverSeed_;

    uint32 zero = 0;

    SHA1 proof;
    proof.Update(session_->GetAccountName());
    proof.Update((uint8*)&zero, sizeof(uint32));
    proof.Update((uint8*)&clientSeed_, sizeof(uint32));
    proof.Update((uint8*)&serverSeed_, sizeof(uint32));
    proof.Update(session_->GetKey());
    proof.Finalize();

    WorldPacket response(CMSG_AUTH_SESSION);
    response << uint32(GameBuild);
    response << uint32(0);
    response << session_->GetAccountName();
    response << uint32(clientSeed_);
    response.append(proof.GetDigest(), proof.GetDigestLength());
    response << uint32(0);

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
    uint8 result;
    recvPacket >> result;

    if (result != AUTH_OK)
    {
        error("Authentication failed with error: %d", result);
        return;
    }

    WorldPacket packet(CMSG_CHAR_ENUM, 0);
    SendPacket(packet);
}