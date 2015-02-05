/*
 * Copyright (C) 2015 Dehravor <dehravor@gmail.com>
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

/** ByteConverter reverse your byte order. This is use
for cross platform where they have different endians.
*/

#pragma once

#include "Define.h"
#include <algorithm>

namespace ByteConverter
{
    template<size_t T>
    inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    template<> inline void convert<0>(char *) { }
    template<> inline void convert<1>(char *) { } // ignore central byte

    template<typename T> inline void apply(T *val)
    {
        convert<sizeof(T)>((char *)(val));
    }

    inline bool IsBigEndian()
    {
        union {
            uint32_t i;
            char c[4];
        } bint = { 0x01020304 };

        return bint.c[0] == 1;
    }
}

template<typename T> inline void EndianConvert(T& val)
{
    if (!ByteConverter::IsBigEndian())
        return;

    ByteConverter::apply<T>(&val);
}

template<typename T> inline void EndianConvertReverse(T& val)
{
    if (ByteConverter::IsBigEndian())
        return;

    ByteConverter::apply<T>(&val);
}

template<typename T> inline void EndianConvertPtr(void* val)
{
    if (!ByteConverter::IsBigEndian())
        return;

    ByteConverter::apply<T>(val);
}

template<typename T> inline void EndianConvertPtrReverse(void* val)
{
    if (ByteConverter::IsBigEndian())
        return;

    ByteConverter::apply<T>(val);
}

template<typename T> void EndianConvert(T*); // will generate link error
template<typename T> void EndianConvertReverse(T*); // will generate link error

inline void EndianConvert(uint8&) { }
inline void EndianConvert(int8&) { }
inline void EndianConvertReverse(uint8&) { }
inline void EndianConvertReverse(int8&) { }
