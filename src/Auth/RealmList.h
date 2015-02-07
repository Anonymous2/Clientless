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
#include "Network/ByteBuffer.h"
#include <vector>
 
enum RealmFlags : uint8
{
    REALM_FLAG_NONE                              = 0x00,
    REALM_FLAG_INVALID                           = 0x01,
    REALM_FLAG_OFFLINE                           = 0x02,
    REALM_FLAG_SPECIFYBUILD                      = 0x04,
    REALM_FLAG_UNK1                              = 0x08,
    REALM_FLAG_UNK2                              = 0x10,
    REALM_FLAG_RECOMMENDED                       = 0x20,
    REALM_FLAG_NEW                               = 0x40,
    REALM_FLAG_FULL                              = 0x80
};
 
struct Realm
{
    uint32 Icon;
    bool Lock;
    RealmFlags Flags;
    std::string Name;
    std::string Address;
    float Population;
    uint8 Characters;
    uint8 Timezone;
    uint8 ID;
    uint8 MajorVersion;
    uint8 MinorVersion;
    uint8 BugfixVersion;
    uint16 Build;
};
 
class RealmList
{
    public:
        void Populate(uint32 count, ByteBuffer &buffer);
        void Print();

        Realm const* GetRealmByName(std::string name);
    private:
        std::vector<Realm> list_;
};