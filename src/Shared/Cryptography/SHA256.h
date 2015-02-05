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
#include "Common.h"
#include <openssl/evp.h>

class BigNumber;

class SHA256
{
    public:
        SHA256();
        ~SHA256();

        void Update(const uint8* data, int32 len);
        void Update(const std::string &str);
        void Update(BigNumber &bn);
        void Finalize();

        uint8* GetDigest() { return digest_; };
        uint32 GetDigestLength() { return digestLength_; };
    private:
        EVP_MD_CTX ctx_;
        uint8 digest_[EVP_MAX_MD_SIZE];
        uint32 digestLength_;
};