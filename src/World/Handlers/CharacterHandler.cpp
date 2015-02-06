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
#include "CharacterList.h"

void WorldSession::HandleCharacterEnum(WorldPacket &recvPacket)
{
    recvPacket.ReadBits(23);
    recvPacket.ReadBit();

    uint32 count = recvPacket.ReadBits(17);

    if (!count)
    {
        error("%s", "You don't have any characters on this realm. Please create one first!");
        socket_.Disconnect();
        return;
    }

    CharacterList characterlist;
    characterlist.Populate(count, recvPacket);
    characterlist.Print();

    if (Character const* character = characterlist.GetCharacterByName(session_->GetCharacterName()))
    {
        // Copy character structure 
        player_ = Player(*character);

        WorldPacket packet(CMSG_LOAD_SCREEN, 5);
        packet << uint32(player_.MapId);
        packet.WriteBit(0x80);  // unk, from [4.3.4 15595 enUS]
        packet.FlushBits();
        SendPacket(packet);

        packet.Initialize(CMSG_VIOLENCE_LEVEL, 1);
        packet << uint8(0x02);  // violenceLevel, from [4.3.4 15595 enUS]
        SendPacket(packet);

        packet.Initialize(CMSG_PLAYER_LOGIN, 8);
        packet.WriteBit(player_.Guid[2]);
        packet.WriteBit(player_.Guid[3]);
        packet.WriteBit(player_.Guid[0]);
        packet.WriteBit(player_.Guid[6]);
        packet.WriteBit(player_.Guid[4]);
        packet.WriteBit(player_.Guid[5]);
        packet.WriteBit(player_.Guid[1]);
        packet.WriteBit(player_.Guid[7]);

        packet.WriteByteSeq(player_.Guid[2]);
        packet.WriteByteSeq(player_.Guid[7]);
        packet.WriteByteSeq(player_.Guid[0]);
        packet.WriteByteSeq(player_.Guid[3]);
        packet.WriteByteSeq(player_.Guid[5]);
        packet.WriteByteSeq(player_.Guid[6]);
        packet.WriteByteSeq(player_.Guid[1]);
        packet.WriteByteSeq(player_.Guid[4]);
        SendPacket(packet);

        std::shared_ptr<Event> keepAliveEvent(new Event(EVENT_SEND_KEEP_ALIVE));
        keepAliveEvent->SetPeriod(MINUTE * IN_MILLISECONDS);
        keepAliveEvent->SetCallback([this]() {
            WorldPacket packet(CMSG_KEEP_ALIVE, 0);
            SendPacket(packet);
        });

        eventMgr_.AddEvent(keepAliveEvent);

        std::shared_ptr<Event> pingEvent(new Event(EVENT_SEND_PING));
        pingEvent->SetPeriod((MINUTE / 2) * IN_MILLISECONDS);
        pingEvent->SetCallback([this]() {
            SendPing();
        });

        eventMgr_.AddEvent(pingEvent);
    }
    else
    {
        error("There is no character named '%s' on this account! Please choose another one!", session_->GetCharacterName().c_str());
        socket_.Disconnect();
    }
}