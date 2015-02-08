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

#include "Cryptography/BigNumber.h"
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <algorithm>
#include <cstring>

BigNumber::BigNumber()
    : bn_(BN_new())
{ }

BigNumber::BigNumber(BigNumber const& bn)
    : bn_(BN_dup(bn.bn_))
{ }

BigNumber::BigNumber(uint32_t val)
    : bn_(BN_new())
{
    BN_set_word(bn_, val);
}

BigNumber::BigNumber(uint8_t const* buffer, int32_t length)
    : bn_(BN_new())
{
    SetBinary(buffer, length);
}

BigNumber::~BigNumber()
{
    BN_free(bn_);
}

void BigNumber::SetZero()
{
    BN_zero(bn_);
}

void BigNumber::SetOne()
{
    BN_one(bn_);
}

void BigNumber::SetUInt32(uint32_t value)
{
    BN_set_word(bn_, value);
}

void BigNumber::SetUInt64(uint64_t value)
{
    BN_add_word(bn_, (uint32_t)(value >> 32));
    BN_lshift(bn_, bn_, 32);
    BN_add_word(bn_, (uint32_t)(value & 0xFFFFFFFF));
}

void BigNumber::SetBinary(uint8_t const* bytes, int32_t len)
{
    uint8_t* array = new uint8_t[len];

    for (int i = 0; i < len; i++)
        array[i] = bytes[len - 1 - i];

    BN_bin2bn(array, len, bn_);

    delete[] array;
}
void BigNumber::SetRandom(int32_t length)
{
    BN_rand(bn_, length, 0, 1);
}

void BigNumber::SetHexString(const char* str)
{
    BN_hex2bn(&bn_, str);
}

void BigNumber::Negate()
{
    bn_->neg = (!((bn_)->neg)) & 1;
}

bool BigNumber::IsNegative()
{
    return bn_->neg == 1;
}

bool BigNumber::IsZero()
{
    return BN_is_zero(bn_);
}

bool BigNumber::IsOne()
{
    return BN_is_one(bn_);
}

bool BigNumber::IsOdd()
{
    return BN_is_odd(bn_);
}

bool BigNumber::IsEven()
{
    return !BN_is_odd(bn_);
}

bool BigNumber::operator==(BigNumber const& bn)
{
    return BN_cmp(bn_, bn.bn_) == 0;
}

bool BigNumber::operator>(BigNumber const& bn)
{
    return BN_cmp(bn_, bn.bn_) == 1;
}

bool BigNumber::operator<(BigNumber const& bn)
{
    return BN_cmp(bn_, bn.bn_) == -1;
}

BigNumber& BigNumber::operator=(BigNumber const& bn)
{
    if (this == &bn)
        return *this;

    BN_copy(bn_, bn.bn_);
    return *this;
}

BigNumber BigNumber::operator+=(BigNumber const& bn)
{
    BN_add(bn_, bn_, bn.bn_);
    return *this;
}

BigNumber BigNumber::operator-=(BigNumber const& bn)
{
    BN_sub(bn_, bn_, bn.bn_);
    return *this;
}

BigNumber BigNumber::operator*=(BigNumber const& bn)
{
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_mul(bn_, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

BigNumber BigNumber::operator/=(BigNumber const& bn)
{
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_div(bn_, nullptr, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

BigNumber BigNumber::operator%=(BigNumber const& bn)
{
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_mod(bn_, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

BigNumber BigNumber::Exp(BigNumber const& bn)
{
    BigNumber ret;
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_exp(ret.bn_, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return ret;
}

BigNumber BigNumber::ModExp(BigNumber const& bn1, BigNumber const& bn2)
{
    BigNumber ret;
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_mod_exp(ret.bn_, bn_, bn1.bn_, bn2.bn_, bnctx);
    BN_CTX_free(bnctx);

    return ret;
}

int32_t BigNumber::GetNumBytes(void) const
{
    return BN_num_bytes(bn_);
}

uint32_t BigNumber::AsDword()
{
    return (uint32_t)BN_get_word(bn_);
}

bool BigNumber::isZero() const
{
    return BN_is_zero(bn_);
}

std::unique_ptr<uint8_t[]> BigNumber::AsByteArray(int32_t minSize, bool littleEndian) const
{
    int length = (minSize >= GetNumBytes()) ? minSize : GetNumBytes();

    uint8_t* array = new uint8_t[length];

    // If we need more bytes than length of BigNumber set the rest to 0
    if (length > GetNumBytes())
        memset((void*)array, 0, length);

    BN_bn2bin(bn_, (unsigned char *)array);

    // openssl's BN stores data internally in big endian format, reverse if little endian desired
    if (littleEndian)
        std::reverse(array, array + length);

    std::unique_ptr<uint8_t[]> ret(array);
    return ret;
}

char* BigNumber::AsHexStr() const
{
    return BN_bn2hex(bn_);
}

char* BigNumber::AsDecStr() const
{
    return BN_bn2dec(bn_);
}

