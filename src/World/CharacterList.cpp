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

        character.Guid[3] = recvPacket.ReadBit();
        character.GuildGuid[1] = recvPacket.ReadBit();
        character.GuildGuid[7] = recvPacket.ReadBit();
        character.GuildGuid[2] = recvPacket.ReadBit();
        character.Name.resize(recvPacket.ReadBits(7));
        character.Guid[4] = recvPacket.ReadBit();
        character.Guid[7] = recvPacket.ReadBit();
        character.GuildGuid[3] = recvPacket.ReadBit();
        character.Guid[5] = recvPacket.ReadBit();
        character.GuildGuid[6] = recvPacket.ReadBit();
        character.Guid[1] = recvPacket.ReadBit();
        character.GuildGuid[5] = recvPacket.ReadBit();
        character.GuildGuid[4] = recvPacket.ReadBit();
        character.IsFirstLogin = recvPacket.ReadBit();
        character.Guid[0] = recvPacket.ReadBit();
        character.Guid[2] = recvPacket.ReadBit();
        character.Guid[6] = recvPacket.ReadBit();
        character.GuildGuid[0] = recvPacket.ReadBit();

        list_[i] = character;
    }

    for (uint8 i = 0; i < count; i++)
    {
        recvPacket.read((uint8*)&list_[i].Class, 1);

        for (uint8 j = 0; j < 19; j++)
        {
            recvPacket >> list_[i].Items[j].InventoryType;
            recvPacket >> list_[i].Items[j].DisplayId;
            recvPacket >> list_[i].Items[j].EnchantAuraId;
        }

        for (uint8 j = 0; j < 4; j++)
        {
            recvPacket >> list_[i].Bags[j].DisplayId;
            recvPacket >> list_[i].Bags[j].EnchantId;
            recvPacket >> list_[i].Bags[j].InventoryType;
        }

        recvPacket >> list_[i].Pet.Family;
        recvPacket.ReadByteSeq(list_[i].GuildGuid[2]);
        recvPacket >> list_[i].Slot;
        recvPacket >> list_[i].Display.HairStyle;
        recvPacket.ReadByteSeq(list_[i].GuildGuid[3]);
        recvPacket >> list_[i].Pet.DisplayId;
        recvPacket >> list_[i].Flags;
        recvPacket >> list_[i].Display.HairColor;
        recvPacket.ReadByteSeq(list_[i].Guid[4]);
        recvPacket >> list_[i].MapId;
        recvPacket.ReadByteSeq(list_[i].GuildGuid[5]);
        recvPacket >> list_[i].Position.Z;
        recvPacket.ReadByteSeq(list_[i].GuildGuid[6]);
        recvPacket >> list_[i].Pet.Level;
        recvPacket.ReadByteSeq(list_[i].Guid[3]);
        recvPacket >> list_[i].Position.Y;
        recvPacket >> list_[i].CustomizationFlags;
        recvPacket >> list_[i].Display.FacialHair;
        recvPacket.ReadByteSeq(list_[i].Guid[7]);
        recvPacket.read((uint8*)&list_[i].Gender, 1);
        list_[i].Name = recvPacket.ReadString(list_[i].Name.size());
        recvPacket >> list_[i].Display.Face;
        recvPacket.ReadByteSeq(list_[i].Guid[0]);
        recvPacket.ReadByteSeq(list_[i].Guid[2]);
        recvPacket.ReadByteSeq(list_[i].GuildGuid[1]);
        recvPacket.ReadByteSeq(list_[i].GuildGuid[7]);
        recvPacket >> list_[i].Position.X;
        recvPacket >> list_[i].Display.Skin;
        recvPacket.read((uint8*)&list_[i].Race, 1);
        recvPacket >> list_[i].Level;
        recvPacket.ReadByteSeq(list_[i].Guid[6]);
        recvPacket.ReadByteSeq(list_[i].GuildGuid[4]);
        recvPacket.ReadByteSeq(list_[i].GuildGuid[0]);
        recvPacket.ReadByteSeq(list_[i].Guid[5]);
        recvPacket.ReadByteSeq(list_[i].Guid[1]);
        recvPacket >> list_[i].AreaId;
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