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
#include "Network/TCPSocket.h"
#include "Cryptography/PacketRC4.h"
#include "WorldPacket.h"
#include <queue>
#include <thread>
#include <mutex>

class WorldSession;

class WorldSocket : public TCPSocket
{
    public:
        WorldSocket(WorldSession* session);
        ~WorldSocket();

        bool Connect(std::string address) override;
        void Disconnect() override;

        void EnqueuePacket(WorldPacket &packet);
        std::shared_ptr<WorldPacket> GetNextPacket();
    private:
        void RunSenderThread();
        void RunReceiverThread();

    private:
        WorldSession* session_;

        std::thread senderThread_;
        std::recursive_mutex sendMutex_;
        std::queue<std::shared_ptr<WorldPacket>> sendQueue_;

        std::thread receiverThread_;
        std::recursive_mutex receiveMutex_;
        std::queue<std::shared_ptr<WorldPacket>> receiveQueue_;

        PacketRC4 packetCrypt_;
};