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

#include "HMACSHA1.h"
#include "BigNumber.h"

HMACSHA1::HMACSHA1(uint8_t* seed, uint32_t len)
{
    HMAC_CTX_init(&ctx_);
    HMAC_Init_ex(&ctx_, seed, len, EVP_sha1(), nullptr);
}

HMACSHA1::~HMACSHA1()
{
    HMAC_CTX_cleanup(&ctx_);
}

void HMACSHA1::Update(const uint8_t* data, int32_t len)
{
    HMAC_Update(&ctx_, data, len);
}

void HMACSHA1::Update(const std::string &str)
{
    Update((const uint8_t*)str.c_str(), str.length());
}

void HMACSHA1::Update(const BigNumber &bn)
{
    Update(bn.AsByteArray().get(), bn.GetNumBytes());
}

void HMACSHA1::Finalize()
{
    HMAC_Final(&ctx_, digest_, &digestLength_);
}
