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
#include "Common.h"
#include "SharedDefines.h"
#include <vector>

#pragma pack(push, 1)

struct PlayerNameEntry
{
    uint64_t GUID;
    char Name[13];
    Races PlayerRace;
    Genders PlayerGender;
    Classes PlayerClass;
};

#pragma pack(pop)

template <typename T>
class Cache
{
    public:
        Cache(const std::string fileName);

        bool Load();
        bool Save();

        void Add(T& value);
        bool Has(const T& value);
        const T* Get(const T& value);
        void Remove(const T& value);

    private:
        std::vector<T> entries_;
        std::string fileName_;
};