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

#include "RealmList.h"
#include "Config.h"

void RealmList::Populate(uint32 count, ByteBuffer &buffer)
{
    list_.clear();
    list_.resize(count);

    for (uint32 i = 0; i < count; i++)
    {
        Realm realm;

        buffer >> realm.Icon;
        buffer >> realm.Lock;
        realm.Flags = buffer.read<RealmFlags>();
        buffer >> realm.Name;
        buffer >> realm.Address;
        buffer >> realm.Population;
        buffer >> realm.Characters;
        buffer >> realm.Timezone;
        buffer >> realm.ID;

        if (realm.Flags & REALM_FLAG_SPECIFYBUILD)
        {
            buffer >> realm.MajorVersion;
            buffer >> realm.MinorVersion;
            buffer >> realm.BugfixVersion;
            buffer >> realm.Build;
        }

        list_[i] = realm;
    }
}

void RealmList::Print()
{
    print("%s", "[Realmlist]");

    for (Realm const& realm : list_)
    {
        if (realm.Flags & REALM_FLAG_SPECIFYBUILD)
        {
            if (realm.MajorVersion != GameVersion[0] || realm.MinorVersion != GameVersion[1] || realm.BugfixVersion != GameVersion[2] || realm.Build != GameBuild)
                continue;
        }

        print(" - %s [%s] (%s)", realm.Name.c_str(), realm.Address.c_str(), realm.Flags & REALM_FLAG_OFFLINE ? "Offline" : "Online");
    }
}

Realm const* RealmList::GetRealmByName(std::string name)
{
    for (Realm const& realm : list_)
    {
        if (realm.Name == name)
            return &realm;
    }

    return nullptr;
}