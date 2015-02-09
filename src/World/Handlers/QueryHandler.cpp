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

void WorldSession::SendNameQuery(ObjectGuid guid)
{
    PlayerNameEntry entry;
    entry.GUID = guid.GetRawValue();

    if (playerNames_.Has(entry))
        return;

    WorldPacket packet(CMSG_NAME_QUERY);
    packet << guid;
    SendPacket(packet);
}

void WorldSession::HandleNameQueryResponse(WorldPacket &recvPacket)
{
    PlayerNameEntry entry;
    recvPacket.readPackGUID(entry.GUID);

    bool isUnknown;
    recvPacket >> isUnknown;

    // Every normal server..
    if (isUnknown)
        return;

    std::string name;
    recvPacket >> name;

    // ..and Tauri WoW Server ("<nem létezõ karakter>" if it doesn't exist)
    if (name.find('<') != std::string::npos && name.find('>') != std::string::npos)
        return;

    std::string realmName;
    recvPacket >> realmName;

    assert(name.length() <= 12);
    memcpy(&entry.Name, name.c_str(), name.size() + 1);

    entry.PlayerRace = recvPacket.read<Races>();
    entry.PlayerGender = recvPacket.read<Genders>();
    entry.PlayerClass = recvPacket.read<Classes>();

    // Add to cache
    if (!playerNames_.Has(entry))
        playerNames_.Add(entry);
}