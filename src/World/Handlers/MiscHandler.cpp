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
#include <bitset>

void WorldSession::HandleMOTD(WorldPacket &recvPacket)
{
    uint32 lineCount;
    recvPacket >> lineCount;

    for (uint32 i = 0; i < lineCount; i++)
        std::cout << recvPacket.read<std::string>() << std::endl;
}

void WorldSession::HandlePong(WorldPacket &recvPacket)
{
    uint32 ping;
    recvPacket >> ping;

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    ping_ = uint32(lastPingTime_ - ms.count());
    lastPingTime_ = 0;
}

void WorldSession::SendPing()
{
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    lastPingTime_ = ms.count();

    WorldPacket packet(CMSG_PING);
    packet << uint32(ping_);
    packet << uint32(ping_ / 2);
    SendPacket(packet);
}

void WorldSession::HandleTimeSyncRequest(WorldPacket &recvPacket)
{
    uint32 timeSyncCounter;
    recvPacket >> timeSyncCounter;

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    WorldPacket packet(CMSG_TIME_SYNC_RESP, 8);
    packet << timeSyncCounter;
    packet << uint32(ms.count());
    SendPacket(packet);
}