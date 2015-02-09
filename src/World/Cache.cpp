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

#include "Cache.h"
#include <fstream>
#include <algorithm>

template <typename T>
Cache<T>::Cache(const std::string fileName)
{
    fileName_ = fileName;
}

template <typename T>
bool Cache<T>::Load()
{
    std::ifstream input(fileName_);

    if (!input)
        return false;

    T entry;
    input.read(reinterpret_cast<char*>(&entry), sizeof(T));

    while (!input.eof())
    {
        entries_.push_back(entry);
        input.read(reinterpret_cast<char*>(&entry), sizeof(T));
    }

    input.close();
    return true;
}

template <typename T>
bool Cache<T>::Save()
{
    std::ofstream output(fileName_);

    if (!output)
        return false;

    for (const T& entry : entries_)
        output.write(reinterpret_cast<const char*>(&entry), sizeof(T));

    if (!output)
        return false;

    return true;
}

template <typename T>
void Cache<T>::Add(T& value)
{
    entries_.push_back(value);
}

template <typename T>
bool Cache<T>::Has(const T& value)
{
    int count = std::count_if(entries_.begin(), entries_.end(), [value](const T& entry) {
        return entry.GUID == value.GUID;
    });

    return count > 0;
}

template <typename T>
const T* Cache<T>::Get(const T& value)
{
    auto itr = std::find_if(entries_.begin(), entries_.end(), [value](const T& entry) {
        return entry.GUID == value.GUID;
    });

    if (itr == entries_.end())
        return nullptr;

    return &(*itr);
}

template <typename T>
void Cache<T>::Remove(const T& value)
{
    std::remove_if(entries_.begin(), entries_.end(), [value](const T& entry) {
        return entry.GUID == value.GUID;
    });
}

template class Cache<PlayerNameEntry>;