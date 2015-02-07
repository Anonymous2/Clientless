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
#include "Session.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "SharedDefines.h"
#include "Player.h"
#include "ObjectGuid.h"
#include "EventMgr.h"
#include "WorldSocket.h"
#include <queue>

struct WorldOpcodeHandler;

class WorldSession
{
    friend class WorldSocket;

    public:
        WorldSession(std::shared_ptr<Session> session);
        ~WorldSession();

        void Enter();
        void HandleConsoleCommand(std::string cmd);

        WorldSocket* GetSocket();
    private:
        std::shared_ptr<Session> session_;
        uint32 clientSeed_;
        uint32 serverSeed_;

        WorldSocket socket_;
        EventMgr eventMgr_;
        Player player_;

        uint64 lastPingTime_;
        uint32 ping_;

        const std::vector<WorldOpcodeHandler> GetOpcodeHandlers();
        void HandlePacket(std::shared_ptr<WorldPacket> recvPacket);
        void SendPacket(WorldPacket &packet);

    // AuthHandler.cpp
    private:
        void HandleAuthenticationChallenge(WorldPacket &recvPacket);
        void HandleAuthenticationResponse(WorldPacket &recvPacket);

    // CharacterHandler.cpp
    private:
        void HandleCharacterEnum(WorldPacket &recvPacket);

    // MiscHandler.cpp
    private:
        void HandleMOTD(WorldPacket &recvPacket);
        void HandlePong(WorldPacket &recvPacket);
        void SendPing();
        void HandleTimeSyncRequest(WorldPacket &recvPacket);
};