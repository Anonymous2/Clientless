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

#pragma once

#include "RC4.h"

class BigNumber;

class PacketRC4
{
    public:
        PacketRC4();
        ~PacketRC4();

        void Initialize(const BigNumber* key);
        void DecryptReceived(uint8* data, int32 len);
        void EncryptSend(uint8* data, int32 len);
        bool IsInitialized() { return ready_; }

    private:
        bool ready_;
        RC4 decrypt_;
        RC4 encrypt_;
};