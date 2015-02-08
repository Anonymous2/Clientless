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

#include "Define.h"
#include "RC4.h"
#include "BigNumber.h"

class WardenRC4
{
    public:
        WardenRC4();
        ~WardenRC4();

        void Initialize(const BigNumber* key);
        bool IsInitialized();
        
        void Encrypt(uint8* data, uint32 size);
        void Decrypt(uint8* data, uint32 size);

    private:
        bool ready_;
        uint32 taken_;
        uint8 buffer_[3][20];
        RC4 encryptionStream_;
        RC4 decryptionStream_;

        void FillUp();
        void Generate(uint8* buffer, uint32 size);
};