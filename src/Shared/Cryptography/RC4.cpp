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

#include "RC4.h"

RC4::RC4(int32_t len) : ctx_()
{
    EVP_CIPHER_CTX_init(&ctx_);
    EVP_EncryptInit_ex(&ctx_, EVP_rc4(), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_set_key_length(&ctx_, len);
}

RC4::RC4(uint8_t* seed, int32_t len) : ctx_()
{
    EVP_CIPHER_CTX_init(&ctx_);
    EVP_EncryptInit_ex(&ctx_, EVP_rc4(), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_set_key_length(&ctx_, len);
    EVP_EncryptInit_ex(&ctx_, nullptr, nullptr, seed, nullptr);
}

RC4::~RC4()
{
    EVP_CIPHER_CTX_cleanup(&ctx_);
}

void RC4::Initialize(uint8_t* seed)
{
    EVP_EncryptInit_ex(&ctx_, nullptr, nullptr, seed, nullptr);
}

void RC4::Update(uint8_t* data, int32_t len)
{
    int32_t outlen = 0;
    EVP_EncryptUpdate(&ctx_, data, &outlen, data, len);
    EVP_EncryptFinal_ex(&ctx_, data, &outlen);
}
