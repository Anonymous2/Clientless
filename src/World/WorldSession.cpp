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
#include <functional>
#include <limits>
#include <iostream>
#include <future>
#include "EventMgr.h"

struct WorldOpcodeHandler
{
    Opcodes opcode;
    void (WorldSession::*handler)(WorldPacket &recvPacket);
};

WorldSession::WorldSession(std::shared_ptr<Session> session) : session_(session), socket_(this), serverSeed_(0), ping_(0), lastPingTime_(0)
{
    clientSeed_ = static_cast<uint32>(time(nullptr));
}

WorldSession::~WorldSession()
{

}

WorldOpcodeHandler* WorldSession::GetOpcodeHandlers() const
{
    static WorldOpcodeHandler table[] =
    {
        // AuthHandler.cpp
        { MSG_VERIFY_CONNECTIVITY, &WorldSession::HandleConnectionVerification },
        { SMSG_AUTH_CHALLENGE, &WorldSession::HandleAuthChallenge },
        { SMSG_AUTH_RESPONSE, &WorldSession::HandleAuthResponse },
        { SMSG_REDIRECT_CLIENT, &WorldSession::HandleRedirect },

        // CharacterHandler.cpp
        { SMSG_CHAR_ENUM, &WorldSession::HandleCharacterEnum },

        // MiscHandler.cpp
        { SMSG_MOTD, &WorldSession::HandleMOTD },
        { SMSG_PONG, &WorldSession::HandlePong },
        { SMSG_TIME_SYNC_REQ, &WorldSession::HandleTimeSyncRequest },

        { NULL_OPCODE, nullptr }
    };
 
    return table;
}

void WorldSession::HandlePacket(std::shared_ptr<WorldPacket> recvPacket)
{
    static WorldOpcodeHandler* table = GetOpcodeHandlers();
 
    for (uint16 i = 0; table[i].handler != nullptr; i++)
    {
        if (table[i].opcode == recvPacket->GetOpcode())
        {
            try
            {
                (this->*table[i].handler)(*recvPacket);
            }
            catch (ByteBufferException const& exception)
            {
                error("%s", "ByteBufferException occured while handling a world packet!");
                error("Opcode: 0x%04x", recvPacket->GetOpcode());
                error("%s", exception.what());
            }

            break;
        }
    }
}

void WorldSession::SendPacket(WorldPacket &packet)
{
    socket_.EnqueuePacket(packet);
}

void WorldSession::Enter()
{
    if (!socket_.Connect(session_->GetRealm().Address))
        return;
    
    eventMgr_.Stop();
    {
        std::shared_ptr<Event> packetProcessEvent(new Event(EVENT_PROCESS_INCOMING));
        packetProcessEvent->SetPeriod(10);
        packetProcessEvent->SetEnabled(true);
        packetProcessEvent->SetCallback([this]() {
            std::shared_ptr<WorldPacket> packet = socket_.GetNextPacket();

            if (packet)
                HandlePacket(packet);
        });

        eventMgr_.AddEvent(packetProcessEvent);

        std::shared_ptr<Event> keepAliveEvent(new Event(EVENT_SEND_KEEP_ALIVE));
        keepAliveEvent->SetPeriod(MINUTE * IN_MILLISECONDS);
        keepAliveEvent->SetEnabled(false);
        keepAliveEvent->SetCallback([this]() {
            WorldPacket packet(CMSG_KEEP_ALIVE, 0);
            SendPacket(packet);
        });

        eventMgr_.AddEvent(keepAliveEvent);

        std::shared_ptr<Event> pingEvent(new Event(EVENT_SEND_PING));
        pingEvent->SetPeriod((MINUTE / 2) * IN_MILLISECONDS);
        pingEvent->SetEnabled(false);
        pingEvent->SetCallback([this]() {
            SendPing();
        });

        eventMgr_.AddEvent(pingEvent);
    }
    eventMgr_.Start();
}

void WorldSession::HandleConsoleCommand(std::string cmd)
{
    if (cmd == "quit" || cmd == "disconnect" || cmd == "logout")
        socket_.Disconnect();
}

WorldSocket* WorldSession::GetSocket()
{
    return &socket_;
}