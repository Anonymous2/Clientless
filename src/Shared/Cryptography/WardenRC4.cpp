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

#include "WardenRC4.h"
#include "SHA1.h"
#include <cstring>

WardenRC4::WardenRC4() : ready_(false), encryptionStream_(16), decryptionStream_(16)
{
}

WardenRC4::~WardenRC4()
{
}

void WardenRC4::Initialize(const BigNumber* key)
{
    uint32 half = key->GetNumBytes() / 2;

    memset(buffer_[0], 0, 20);

    SHA1 firstHash;
    firstHash.Update(key->AsByteArray().get(), half);
    firstHash.Finalize();

    uint32 dglen = firstHash.GetDigestLength();
    memcpy(buffer_[1], firstHash.GetDigest(), firstHash.GetDigestLength());

    SHA1 secondHash;
    secondHash.Update(key->AsByteArray().get() + half, half);
    secondHash.Finalize();

    memcpy(buffer_[2], secondHash.GetDigest(), secondHash.GetDigestLength());

    FillUp();

    uint8 encryptionKey[16], decryptionKey[16];

    Generate(&encryptionKey[0], 16);
    encryptionStream_.Initialize(encryptionKey);

    Generate(&decryptionKey[0], 16);
    decryptionStream_.Initialize(decryptionKey);

    ready_ = true;
}

bool WardenRC4::IsInitialized()
{
    return ready_;
}

void WardenRC4::Encrypt(uint8* data, uint32 size)
{
    if (!ready_)
        return;
    
    encryptionStream_.Update(data, size);
}

void WardenRC4::Decrypt(uint8* data, uint32 size)
{
    if (!ready_)
        return;
    
    decryptionStream_.Update(data, size);
}

void WardenRC4::FillUp()
{
    SHA1 hash;
    hash.Update(buffer_[1], 20);
    hash.Update(buffer_[0], 20);
    hash.Update(buffer_[2], 20);
    hash.Finalize();

    memcpy(buffer_[0], hash.GetDigest(), hash.GetDigestLength());

    taken_ = 0;
}

void WardenRC4::Generate(uint8* buffer, uint32 size)
{
    for (uint32 i = 0; i < size; ++i)
    {
        if (taken_ == 20)
            FillUp();

        assert(taken_ < 20);

        buffer[i] = buffer_[0][taken_];
        taken_++;
    }
}