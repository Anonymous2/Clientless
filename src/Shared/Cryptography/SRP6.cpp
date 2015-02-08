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

#include "SRP6.h"
#include <algorithm>

SRP6::SRP6()
{
}

SRP6::~SRP6()
{
}

void SRP6::Reset()
{
    AccountName = "";
    AccountPassword = "";
    N.SetZero();
    g.SetZero();
    k.SetUInt32(3);
    a.SetRandom(19 * 8);
    B.SetZero();
    s.SetZero();
    A.SetZero();
    I.SetZero();
    x.SetZero();
    u.SetZero();
    S.SetZero();
    K.SetZero();
    M1.SetZero();
    M2.SetZero();
}

void SRP6::SetCredentials(std::string name, std::string password)
{
    AccountName = name;
    AccountPassword = password;
}
void SRP6::SetServerModulus(uint8_t* buffer, uint32_t length)
{
    N.SetBinary(buffer, length);
}

void SRP6::SetServerGenerator(uint8_t* buffer, uint32_t length)
{
    g.SetBinary(buffer, length);
}

void SRP6::SetServerEphemeralB(uint8_t* buffer, uint32_t length)
{
    B.SetBinary(buffer, length);
}

void SRP6::SetServerSalt(uint8_t* buffer, uint32_t length)
{
    s.SetBinary(buffer, length);
}

void SRP6::Calculate()
{
    // Safeguards

    if (B.IsZero() || (B % N).IsZero())
    {
        std::cerr << "SRP safeguard: B (mod N) was zero!" << std::endl;
        return;
    }

    if (a > N || a == N)
    {
        std::cerr << "SRP safeguard: a must be less than N!" << std::endl;
        return;
    }

    // I = H(g) xor H(N)

    SHA1 hg;
    hg.Update(g);
    hg.Finalize();

    SHA1 hN;
    hN.Update(N);
    hN.Finalize();

    uint8_t bI[20];

    for (uint32_t i = 0; i < 20; i++)
        bI[i] = hg.GetDigest()[i] ^ hN.GetDigest()[i];

    I.SetBinary(bI, 20);

    // x = H(s, H(C, ":", P));

    SHA1 hCredentials;
    hCredentials.Update(AccountName + ":" + AccountPassword);
    hCredentials.Finalize();

    SHA1 hx;
    hx.Update(s);
    hx.Update(hCredentials.GetDigest(), hCredentials.GetDigestLength());
    hx.Finalize();

    x.SetBinary(hx.GetDigest(), hx.GetDigestLength());

    // A

    A = g.ModExp(a, N);

    // u = H(A, B)

    SHA1 hu;
    hu.Update(A);
    hu.Update(B);
    hu.Finalize();

    u.SetBinary(hu.GetDigest(), hu.GetDigestLength());

    if (u.IsZero())
    {
        std::cerr << "SRP safeguard: 'u' must not be zero!" << std::endl;
        return;
    }

    // S

    S = (B - k * g.ModExp(x, N)).ModExp((a + u * x), N);

    if (S.IsZero() || S.IsNegative())
    {
        std::cerr << "SRP safeguard: S must be greater than 0!" << std::endl;
        return;
    }

    // K

    uint8_t SPart[2][16];

    for (int i = 0; i < 16; i++)
    {
        SPart[0][i] = S.AsByteArray()[i * 2];
        SPart[1][i] = S.AsByteArray()[i * 2 + 1];
    }

    SHA1 hEven;
    hEven.Update(SPart[0], 16);
    hEven.Finalize();

    SHA1 hOdd;
    hOdd.Update(SPart[1], 16);
    hOdd.Finalize();

    uint8_t bK[40];

    for (uint32_t i = 0; i < 20; i++)
    {
        bK[i * 2] = hEven.GetDigest()[i];
        bK[i * 2 + 1] = hOdd.GetDigest()[i];
    }

    K.SetBinary(bK, sizeof(bK));

    // M1 = H(I, H(C), s, A, B, K)

    SHA1 hUsername;
    hUsername.Update(AccountName);
    hUsername.Finalize();

    SHA1 hM1;
    hM1.Update(I);
    hM1.Update(hUsername.GetDigest(), hUsername.GetDigestLength());
    hM1.Update(s);
    hM1.Update(A);
    hM1.Update(B);
    hM1.Update(K);
    hM1.Finalize();

    M1.SetBinary(hM1.GetDigest(), hM1.GetDigestLength());

    // M2 = H(A, M1, K)

    SHA1 hM2;
    hM2.Update(A);
    hM2.Update(M1);
    hM2.Update(K);
    hM2.Finalize();

    M2.SetBinary(hM2.GetDigest(), hM2.GetDigestLength());

/*
    print("%s", "N: %s", N.AsHexString().c_str());
    print("%s", "g: %s", g.AsHexString().c_str());
    print("%s", "k: %s", k.AsHexString().c_str());
    print("%s", "B: %s", B.AsHexString().c_str());
    print("%s", "s: %s", s.AsHexString().c_str());
    print("%s", "a: %s", a.AsHexString().c_str());
    print("%s", "A: %s", A.AsHexString().c_str());
    print("%s", "I: %s", I.AsHexString().c_str());
    print("%s", "x: %s", x.AsHexString().c_str());
    print("%s", "u: %s", u.AsHexString().c_str());
    print("%s", "S: %s", S.AsHexString().c_str());
    print("%s", "K: %s", K.AsHexString().c_str());
    print("%s", "M1: %s", M1.AsHexString().c_str());
    print("%s", "M2: %s", M2.AsHexString().c_str());
*/
}

bool SRP6::IsValidM2(uint8_t* buffer, uint32_t length)
{
    BigNumber temp(buffer, length);
    return temp == M2;
}
