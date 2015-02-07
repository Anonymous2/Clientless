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
#include "Common.h"
#include "Position.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"

struct CharacterDisplay
{
    uint8 Skin;
    uint8 Face;
    uint8 HairStyle;
    uint8 HairColor;
    uint8 FacialHair;
};

struct CharacterPet
{
    uint32 DisplayId;
    uint32 Level;
    uint32 Family;
};

struct CharacterItems
{
    uint32 DisplayId;
    uint8 InventoryType;
};

struct CharacterBags
{
    uint32 DisplayId;
    uint8 InventoryType;
    uint32 EnchantId;
};

struct Character
{
    ObjectGuid Guid;
    std::string Name;
    Races Race;
    Classes Class;
    Genders Gender;
    CharacterDisplay Display;
    uint8 Level;
    uint32 AreaId;
    uint32 MapId;
    Position4D Position;
    uint32 GuildId;
    uint32 Flags;
    uint32 CustomizationFlags;
    uint8 Slot;
    bool IsFirstLogin;
    CharacterPet Pet;
    CharacterItems Items[19];
    CharacterBags Bags[4];

    bool IsAlliance() const
    {
        switch (Race)
        {
            case RACE_HUMAN:
            case RACE_NIGHTELF:
            case RACE_GNOME:
            case RACE_DWARF:
            case RACE_DRAENEI:
            case RACE_WORGEN:
                return true;
            default:
                return false;
        }

        return false;
    }

    bool IsHorde() const
    {
        return !IsAlliance();
    }

    std::string GetRaceAsStr() const
    {
        switch (Race)
        {
            case RACE_HUMAN:
                return "Human";
            case RACE_ORC:
                return "Orc";
            case RACE_DWARF:
                return "Dwarf";
            case RACE_NIGHTELF:
                return "Night elf";
            case RACE_UNDEAD_PLAYER:
                return "Undead";
            case RACE_TAUREN:
                return "Tauren";
            case RACE_GNOME:
                return "Gnome";
            case RACE_TROLL:
                return "Troll";
            case RACE_BLOODELF:
                return "Blood elf";
            case RACE_DRAENEI:
                return "Draenei";
            case RACE_GOBLIN:
                return "Goblin";
            case RACE_WORGEN:
                return "Worgen";
            default:
                return "Unknown";
        }

        return "";
    }

    std::string GetClassAsStr() const
    {
        switch (Class)
        {
            case CLASS_WARRIOR:
                return "Warrior";
            case CLASS_PALADIN:
                return "Paladin";
            case CLASS_HUNTER:
                return "Hunter";
            case CLASS_ROGUE:
                return "Rogue";
            case CLASS_PRIEST:
                return "Priest";
            case CLASS_DEATH_KNIGHT:
                return "Death Knight";
            case CLASS_SHAMAN:
                return "Shaman";
            case CLASS_MAGE:
                return "Mage";
            case CLASS_WARLOCK:
                return "Warlock";
            case CLASS_DRUID:
                return "Druid";
            default:
                return "Unknown";
        }

        return "";
    }
};