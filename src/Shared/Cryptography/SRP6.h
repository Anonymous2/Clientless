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
#include "Common.h"
#include "Cryptography/BigNumber.h"
#include "Cryptography/SHA1.h"

class SRP6
{
    public:
        SRP6();
        ~SRP6();

        void Reset();
        void SetCredentials(std::string name, std::string password);
        void SetServerModulus(uint8* buffer, uint32 length);
        void SetServerGenerator(uint8* buffer, uint32 length);
        void SetServerEphemeralB(uint8* buffer, uint32 length);
        void SetServerSalt(uint8* buffer, uint32 length);
        void Calculate();
        bool IsValidM2(uint8* buffer, uint32 length);

        BigNumber* GetClientEphemeralA() { return &A; }
        BigNumber* GetClientM1() { return &M1; }
        BigNumber* GetClientK() { return &K; }

    private:
        std::string AccountName;
        std::string AccountPassword;

        BigNumber N; // Modulus
        BigNumber g; // Generator
        BigNumber k; // Multiplier
        BigNumber B; // Server public
        BigNumber s; // Server salt
        BigNumber a; // Client secret
        BigNumber A; // Client public
        BigNumber I; // g hash ^ N hash
        BigNumber x; // Client credentials
        BigNumber u; // Scrambling
        BigNumber S; // Key
        BigNumber K; // Key based on S
        BigNumber M1; // M1
        BigNumber M2; // M2
};
