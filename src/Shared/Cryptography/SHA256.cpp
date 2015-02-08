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

#include "SHA256.h"
#include "BigNumber.h"

SHA256::SHA256() : digestLength_(0)
{
    EVP_MD_CTX_init(&ctx_);
    EVP_DigestInit_ex(&ctx_, EVP_sha256(), nullptr);
}

SHA256::~SHA256()
{
    EVP_MD_CTX_cleanup(&ctx_);
}

void SHA256::Update(const uint8_t* data, int32_t len)
{
    EVP_DigestUpdate(&ctx_, data, len);
}

void SHA256::Update(const std::string &str)
{
    Update((const uint8_t*)str.c_str(), str.length());
}

void SHA256::Update(BigNumber &bn)
{
    Update(bn.AsByteArray().get(), bn.GetNumBytes());
}

void SHA256::Finalize()
{
    EVP_DigestFinal_ex(&ctx_, digest_, &digestLength_);
}
