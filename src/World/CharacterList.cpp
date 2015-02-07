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

void CharacterList::Populate(uint8 count, WorldPacket &recvPacket)
{
    list_.clear();
    list_.resize(count);

    for (uint8 i = 0; i < count; i++)
    {
        Character character;

        recvPacket >> character.Guid;
        recvPacket >> character.Name;
        recvPacket.read((uint8*)&character.Race, 1);
        recvPacket.read((uint8*)&character.Class, 1);
        recvPacket.read((uint8*)&character.Gender, 1);
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
        recvPacket >> character.CustomizationFlags;
        recvPacket >> character.IsFirstLogin;
        recvPacket >> character.Pet.DisplayId;
        recvPacket >> character.Pet.Level;
        recvPacket >> character.Pet.Family;

        for (uint8 j = 0; j < 19; j++)
        {
            recvPacket >> character.Items[j].DisplayId;
            recvPacket >> character.Items[j].InventoryType;
            recvPacket >> character.Items[j].EnchantAuraId;
        }

        for (uint8 j = 0; j < 4; j++)
        {
            recvPacket >> character.Bags[j].DisplayId;
            recvPacket >> character.Bags[j].InventoryType;
            recvPacket >> character.Bags[j].EnchantId;
        }

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