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

#include "Session.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void Session::RequestData()
{
    std::cout << "[Session]" << std::endl;
    std::cout << "Please enter the address of your server: set realmlist ";
    std::getline(std::cin, authServerAddress_);

    std::cout << "Please enter your account name: ";
    std::getline(std::cin, accountName_);

    std::transform(accountName_.begin(), accountName_.end(), accountName_.begin(), toupper);

    std::cout << "Please enter your account password: ";
    std::getline(std::cin, accountPassword_);

    std::transform(accountPassword_.begin(), accountPassword_.end(), accountPassword_.begin(), toupper);

    std::cout << "Please enter your realm's name: ";
    std::getline(std::cin, realmName_);

    std::cout << "Please enter your character's name: ";
    std::getline(std::cin, characterName_);

    std::string choice;
    std::cout << "Would you like to save these values? [Y/N]: ";
    std::getline(std::cin, choice);

    if (choice == "Y" || choice == "y")
    {
        if (SaveData())
            std::cout << "Successfully saved!" << std::endl;
        else
            std::cerr << "Save failed!" << std::endl;
    }
}

bool Session::LoadSavedData()
{
    std::ifstream file("settings.ini");

    if (!file)
        return false;

    if (!std::getline(file, authServerAddress_))
        return false;

    if (!std::getline(file, accountName_))
        return false;

    std::transform(accountName_.begin(), accountName_.end(), accountName_.begin(), toupper);

    if (!std::getline(file, accountPassword_))
        return false;

    std::transform(accountPassword_.begin(), accountPassword_.end(), accountPassword_.begin(), toupper);

    if (!std::getline(file, realmName_))
        return false;

    if (!std::getline(file, characterName_))
        return false;

    return true;
}

bool Session::SaveData()
{
    std::ofstream file("settings.ini");

    if (!file)
        return false;

    file << authServerAddress_ << std::endl;
    file << accountName_ << std::endl;
    file << accountPassword_ << std::endl;
    file << realmName_ << std::endl;
    file << characterName_ << std::endl;

    if (!file)
        return false;

    return true;
}

void Session::Print()
{
    print("%s", "[Current session]");
    print(" - Authentication server: %s", authServerAddress_.c_str());
    print(" - Account name: %s", accountName_.c_str());
    print(" - Realm name: %s", realmName_.c_str());
    print(" - Character name: %s", characterName_.c_str());
}

std::string Session::GetAuthenticationServerAddress()
{
    return authServerAddress_;
}

std::string Session::GetAccountName()
{
    return accountName_;
}

std::string Session::GetAccountPassword()
{
    return accountPassword_;
}

std::string Session::GetRealmName()
{
    return realmName_;
}

std::string Session::GetCharacterName()
{
    return characterName_;
}

void Session::SetRealm(const Realm& realm)
{
    realm_ = realm;
}

const Realm& Session::GetRealm()
{
    return realm_;
}

void Session::SetKey(const BigNumber& key)
{
    key_ = key;
}

const BigNumber& Session::GetKey()
{
    return key_;
}