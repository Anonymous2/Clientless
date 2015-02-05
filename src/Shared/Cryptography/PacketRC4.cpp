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

#include "PacketRC4.h"
#include "BigNumber.h"
#include "HMACSHA1.h"
#include <cstring>

PacketRC4::PacketRC4() : ready_(false), decrypt_(20), encrypt_(20)
{
}

PacketRC4::~PacketRC4()
{
}

void PacketRC4::Initialize(const BigNumber* key)
{
    // Decryption
    uint8 decryptKey[16] = { 0xCC, 0x98, 0xAE, 0x04, 0xE8, 0x97, 0xEA, 0xCA, 0x12, 0xDD, 0xC0, 0x93, 0x42, 0x91, 0x53, 0x57 };

    HMACSHA1 decryptHMAC(decryptKey, 16);
    decryptHMAC.Update(*key);
    decryptHMAC.Finalize();

    decrypt_.Initialize(decryptHMAC.GetDigest());

    // Encryption
    uint8 encryptKey[16] = { 0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE, 0x2F, 0x43, 0x67, 0xCE };

    HMACSHA1 encryptHMAC(encryptKey, 16);
    encryptHMAC.Update(*key);
    encryptHMAC.Finalize();

    encrypt_.Initialize(encryptHMAC.GetDigest());

    // Drop-N
    uint8 drop[1024];
    memset(drop, 0, 1024);

    encrypt_.Update(drop, 1024);
    decrypt_.Update(drop, 1024);

    ready_ = true;
}

void PacketRC4::DecryptReceived(uint8* data, int32 len)
{
    if (!ready_)
        return;

    decrypt_.Update(data, len);
}

void PacketRC4::EncryptSend(uint8* data, int32 len)
{
    if (!ready_)
        return;

    encrypt_.Update(data, len);
}
