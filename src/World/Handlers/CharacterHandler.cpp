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
    uint8 count;
    recvPacket >> count;

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

        WorldPacket packet(CMSG_PLAYER_LOGIN, 8);
        packet << player_.Guid;
        SendPacket(packet);
    }
    else
    {
        error("There is no character named '%s' on this account! Please choose another one!", session_->GetCharacterName().c_str());
        socket_.Disconnect();
    }
}