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

#pragma once
#include <cstdint>
#include "Cryptography/BigNumber.h"
#include "Auth/RealmList.h"

class Session
{
    public:
        void RequestData();
        bool LoadSavedData();
        bool SaveData();
        void Print();

        std::string GetAuthenticationServerAddress();
        std::string GetAccountName();
        std::string GetAccountPassword();
        std::string GetRealmName();
        std::string GetCharacterName();
        void SetRealm(const Realm& realm);
        const Realm& GetRealm();
        void SetKey(const BigNumber& key);
        const BigNumber& GetKey();

    private:
        std::string authServerAddress_;
        std::string accountName_;
        std::string accountPassword_;
        std::string realmName_;
        std::string characterName_;
        Realm realm_;
        BigNumber key_;
};