/*
 * Copyright (C) 2015 Dehravor <dehravor@gmail.com>
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ObjectGuid_h__
#define ObjectGuid_h__

#include <cstdint>
#include <functional>
#include <cassert>

enum TypeID
{
    TYPEID_OBJECT        = 0,
    TYPEID_ITEM          = 1,
    TYPEID_CONTAINER     = 2,
    TYPEID_UNIT          = 3,
    TYPEID_PLAYER        = 4,
    TYPEID_GAMEOBJECT    = 5,
    TYPEID_DYNAMICOBJECT = 6,
    TYPEID_CORPSE        = 7,
    TYPEID_AREATRIGGER   = 8
};

#define NUM_CLIENT_OBJECT_TYPES             9

enum TypeMask
{
    TYPEMASK_OBJECT         = 0x0001,
    TYPEMASK_ITEM           = 0x0002,
    TYPEMASK_CONTAINER      = 0x0006,                       // TYPEMASK_ITEM | 0x0004
    TYPEMASK_UNIT           = 0x0008,                       // creature
    TYPEMASK_PLAYER         = 0x0010,
    TYPEMASK_GAMEOBJECT     = 0x0020,
    TYPEMASK_DYNAMICOBJECT  = 0x0040,
    TYPEMASK_CORPSE         = 0x0080,
    TYPEMASK_AREATRIGGER    = 0x0100,
    TYPEMASK_SEER           = TYPEMASK_PLAYER | TYPEMASK_UNIT | TYPEMASK_DYNAMICOBJECT
};

enum HighGuid
{
    HIGHGUID_ITEM           = 0x400,                       // blizz 4000
    HIGHGUID_CONTAINER      = 0x400,                       // blizz 4000
    HIGHGUID_PLAYER         = 0x000,                       // blizz 0000
    HIGHGUID_GAMEOBJECT     = 0xF11,                       // blizz F110
    HIGHGUID_TRANSPORT      = 0xF12,                       // blizz F120 (for GAMEOBJECT_TYPE_TRANSPORT)
    HIGHGUID_UNIT           = 0xF13,                       // blizz F130
    HIGHGUID_PET            = 0xF14,                       // blizz F140
    HIGHGUID_VEHICLE        = 0xF15,                       // blizz F550
    HIGHGUID_DYNAMICOBJECT  = 0xF10,                       // blizz F100
    HIGHGUID_CORPSE         = 0xF101,                      // blizz F100
    HIGHGUID_AREATRIGGER    = 0xF102,
    HIGHGUID_BATTLEGROUND   = 0x1F1,
    HIGHGUID_MO_TRANSPORT   = 0x1FC,                       // blizz 1FC0 (for GAMEOBJECT_TYPE_MO_TRANSPORT)
    HIGHGUID_INSTANCE       = 0x1F4,                       // blizz 1F40
    HIGHGUID_GROUP          = 0x1F5,
    HIGHGUID_GUILD          = 0x1FF
};

class ObjectGuid;
class PackedGuid;

struct PackedGuidReader
{
    explicit PackedGuidReader(ObjectGuid& guid) : GuidPtr(&guid) { }
    ObjectGuid* GuidPtr;
};

class ObjectGuid
{
    public:
        static ObjectGuid const Empty;

        ObjectGuid() { _data._guid = 0; }
        explicit ObjectGuid(uint64_t guid)  { _data._guid = guid; }
        ObjectGuid(HighGuid hi, uint32_t entry, uint32_t counter) { _data._guid = counter ? uint64_t(counter) | (uint64_t(entry) << 32) | (uint64_t(hi) << ((hi == HIGHGUID_CORPSE || hi == HIGHGUID_AREATRIGGER) ? 48 : 52)) : 0; }
        ObjectGuid(HighGuid hi, uint32_t counter) { _data._guid = counter ? uint64_t(counter) | (uint64_t(hi) << ((hi == HIGHGUID_CORPSE || hi == HIGHGUID_AREATRIGGER) ? 48 : 52)) : 0; }

        operator uint64_t() const { return _data._guid; }
        PackedGuidReader ReadAsPacked() { return PackedGuidReader(*this); }

        void Set(uint64_t guid) { _data._guid = guid; }
        void Clear() { _data._guid = 0; }

        PackedGuid WriteAsPacked() const;

        uint64_t   GetRawValue() const { return _data._guid; }
        HighGuid GetHigh() const
        {
            uint32_t temp = ((uint64_t(_data._guid) >> 48) & 0x0000FFFF);
            return HighGuid((temp == HIGHGUID_CORPSE || temp == HIGHGUID_AREATRIGGER) ? temp : ((temp >> 4) & 0x00000FFF));
        }
        uint32_t   GetEntry() const { return HasEntry() ? uint32_t((_data._guid >> 32) & 0x00000000000FFFFF) : 0; }
        uint32_t   GetCounter()  const
        {
            return uint32_t(_data._guid & 0x00000000FFFFFFFF);
        }

        static uint32_t GetMaxCounter(HighGuid /*high*/)
        {
            return uint32_t(0xFFFFFFFF);
        }

        uint32_t GetMaxCounter() const { return GetMaxCounter(GetHigh()); }

        uint8_t& operator[](uint32_t index)
        {
            assert(index < sizeof(uint64_t));
            return _data._bytes[index];
        }

        uint8_t const& operator[](uint32_t index) const
        {
            assert(index < sizeof(uint64_t));
            return _data._bytes[index];
        }

        bool IsEmpty()             const { return _data._guid == 0; }
        bool IsCreature()          const { return GetHigh() == HIGHGUID_UNIT; }
        bool IsPet()               const { return GetHigh() == HIGHGUID_PET; }
        bool IsVehicle()           const { return GetHigh() == HIGHGUID_VEHICLE; }
        bool IsCreatureOrPet()     const { return IsCreature() || IsPet(); }
        bool IsCreatureOrVehicle() const { return IsCreature() || IsVehicle(); }
        bool IsAnyTypeCreature()   const { return IsCreature() || IsPet() || IsVehicle(); }
        bool IsPlayer()            const { return !IsEmpty() && GetHigh() == HIGHGUID_PLAYER; }
        bool IsUnit()              const { return IsAnyTypeCreature() || IsPlayer(); }
        bool IsItem()              const { return GetHigh() == HIGHGUID_ITEM; }
        bool IsGameObject()        const { return GetHigh() == HIGHGUID_GAMEOBJECT; }
        bool IsDynamicObject()     const { return GetHigh() == HIGHGUID_DYNAMICOBJECT; }
        bool IsCorpse()            const { return GetHigh() == HIGHGUID_CORPSE; }
        bool IsAreaTrigger()       const { return GetHigh() == HIGHGUID_AREATRIGGER; }
        bool IsBattleground()      const { return GetHigh() == HIGHGUID_BATTLEGROUND; }
        bool IsTransport()         const { return GetHigh() == HIGHGUID_TRANSPORT; }
        bool IsMOTransport()       const { return GetHigh() == HIGHGUID_MO_TRANSPORT; }
        bool IsAnyTypeGameObject() const { return IsGameObject() || IsTransport() || IsMOTransport(); }
        bool IsInstance()          const { return GetHigh() == HIGHGUID_INSTANCE; }
        bool IsGroup()             const { return GetHigh() == HIGHGUID_GROUP; }
        bool IsGuild()             const { return GetHigh() == HIGHGUID_GUILD; }

        static TypeID GetTypeId(HighGuid high)
        {
            switch (high)
            {
                case HIGHGUID_ITEM:         return TYPEID_ITEM;
                //case HIGHGUID_CONTAINER:    return TYPEID_CONTAINER; HIGHGUID_CONTAINER==HIGHGUID_ITEM currently
                case HIGHGUID_UNIT:         return TYPEID_UNIT;
                case HIGHGUID_PET:          return TYPEID_UNIT;
                case HIGHGUID_PLAYER:       return TYPEID_PLAYER;
                case HIGHGUID_GAMEOBJECT:   return TYPEID_GAMEOBJECT;
                case HIGHGUID_DYNAMICOBJECT: return TYPEID_DYNAMICOBJECT;
                case HIGHGUID_CORPSE:       return TYPEID_CORPSE;
                case HIGHGUID_AREATRIGGER:  return TYPEID_AREATRIGGER;
                case HIGHGUID_MO_TRANSPORT: return TYPEID_GAMEOBJECT;
                case HIGHGUID_VEHICLE:      return TYPEID_UNIT;
                // unknown
                case HIGHGUID_INSTANCE:
                case HIGHGUID_BATTLEGROUND:
                case HIGHGUID_GROUP:
                case HIGHGUID_GUILD:
                default:                    return TYPEID_OBJECT;
            }
        }

        TypeID GetTypeId() const { return GetTypeId(GetHigh()); }

        bool operator!() const { return IsEmpty(); }
        bool operator== (ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
        bool operator!= (ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
        bool operator< (ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }

        static char const* GetTypeName(HighGuid high);
        char const* GetTypeName() const { return !IsEmpty() ? GetTypeName(GetHigh()) : "None"; }
        std::string ToString() const;

    private:
        static bool HasEntry(HighGuid high)
        {
            switch (high)
            {
                case HIGHGUID_ITEM:
                case HIGHGUID_PLAYER:
                case HIGHGUID_DYNAMICOBJECT:
                case HIGHGUID_CORPSE:
                case HIGHGUID_MO_TRANSPORT:
                case HIGHGUID_INSTANCE:
                case HIGHGUID_GROUP:
                    return false;
                case HIGHGUID_GAMEOBJECT:
                case HIGHGUID_TRANSPORT:
                case HIGHGUID_UNIT:
                case HIGHGUID_PET:
                case HIGHGUID_VEHICLE:
                default:
                    return true;
            }
        }

        bool HasEntry() const { return HasEntry(GetHigh()); }

        explicit ObjectGuid(uint32_t const&) = delete;                 // no implementation, used to catch wrong type assignment
        ObjectGuid(HighGuid, uint32_t, uint64_t counter) = delete;       // no implementation, used to catch wrong type assignment
        ObjectGuid(HighGuid, uint64_t counter) = delete;               // no implementation, used to catch wrong type assignment

        union
        {
            uint64_t _guid;
            uint8_t _bytes[sizeof(uint64_t)];
        } _data;
};

#endif // ObjectGuid_h__