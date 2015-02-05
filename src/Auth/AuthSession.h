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

#pragma once

#include "Define.h"
#include "AuthResult.h"
#include "AuthCmd.h"
#include "Session.h"
#include "Network/TCPSocket.h"
#include "Cryptography/SRP6.h"

class AuthSession
{
    public:
        AuthSession(std::shared_ptr<Session> session);
        ~AuthSession();

        bool Authenticate();
    private:
        bool SendLogonChallenge();
        bool SendLogonProof();
        bool SendRealmlistRequest();

        bool HandleLogonChallengeResponse();
        bool HandleLogonProofResponse();
        bool HandleRealmlistResponse();

    private:
        std::shared_ptr<Session> session_;
        TCPSocket socket_;
        SRP6 srp6_;

        void SendPacket(ByteBuffer& buffer);

        std::string AuthResultToStr(AuthResult result);
};