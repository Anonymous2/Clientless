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

#include "CharacterList.h"

void CharacterList::Populate(uint32 count, WorldPacket &recvPacket)
{
    list_.clear();
    list_.resize(count);

    for (uint32 i = 0; i < count; i++)
    {
        Character character;
        recvPacket >> character.Guid;
        recvPacket >> character.Name;

        character.Race = recvPacket.read<Races>();
        character.Class = recvPacket.read<Classes>();
        character.Gender = recvPacket.read<Genders>();

        recvPacket >> character.Display.Skin;
        recvPacket >> character.Display.Face;
        recvPacket >> character.Display.HairStyle;
        recvPacket >> character.Display.HairColor;
        recvPacket >> character.Display.FacialHair;
        recvPacket >> character.Level;
        recvPacket >> character.AreaId;
        recvPacket >> character.MapId;
        recvPacket >> character.Position.X;
        recvPacket >> character.Position.Y;
        recvPacket >> character.Position.Z;
        recvPacket >> character.GuildId;
        recvPacket >> character.Flags;
        recvPacket >> character.IsFirstLogin;
        recvPacket >> character.Pet.DisplayId;
        recvPacket >> character.Pet.Level;
        recvPacket >> character.Pet.Family;

        for (int j = 0; j < 19; j++)
        {
            recvPacket >> character.Items[j].DisplayId;
            recvPacket >> character.Items[j].InventoryType;
        }

        recvPacket.read_skip<uint32>();
        recvPacket.read_skip<uint8>();

        list_[i] = character;
    }
}

void CharacterList::Print() const
{
    print("%s", "[Characterlist]");

    for (Character const& character : list_)
        print(" - %s (%s %s) [Level %d]", character.Name.c_str(), character.GetRaceAsStr().c_str(), character.GetClassAsStr().c_str(), character.Level);
}

Character const* CharacterList::GetCharacterByName(std::string name) const
{
    for (auto &character : list_)
    {
        if (character.Name == name)
            return &character;
    }

    return nullptr;
}