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
#include "WorldPacket.h"
#include "Cryptography/WardenRC4.h"
#include <memory>

enum WardenOpcodes : uint8
{
    WARDEN_CMSG_MODULE_MISSING          = 0,
    WARDEN_CMSG_MODULE_OK               = 1,
    WARDEN_CMSG_CHEAT_CHECKS_RESULT     = 2,
    WARDEN_CMSG_MEM_CHECKS_RESULT       = 3,
    WARDEN_CMSG_HASH_RESULT             = 4,
    WARDEN_CMSG_MODULE_FAILED           = 5,

    WARDEN_SMSG_MODULE_USE              = 0,
    WARDEN_SMSG_MODULE_CACHE            = 1,
    WARDEN_SMSG_CHEAT_CHECKS_REQUEST    = 2,
    WARDEN_SMSG_MODULE_INITIALIZE       = 3,
    WARDEN_SMSG_MEM_CHECKS_REQUEST      = 4,
    WARDEN_SMSG_HASH_REQUEST            = 5
};

struct WardenModule
{
    uint8 Hash[32];
    uint8 DecryptionKey[16];
    uint32 Size;
    uint32 DecompressedSize;
    ByteBuffer WMOD;
    ByteBuffer BLL;
    uint8 ServerSeed[16];
};

class WorldSession;

class Warden
{
    public:
        Warden(WorldSession* session);

        void Initialize(const BigNumber* key);
        void PreparePacket(WorldPacket &packet, WardenOpcodes opcode);
        void SendPacket(WorldPacket &packet);
        bool VerifyModule();
        void DecompressModule();
        void InitializeModule();

        void HandleData(WorldPacket &recvPacket);
        void HandleModuleInfo(WorldPacket &recvPacket);
        void HandleModuleData(WorldPacket &recvPacket);
        void HandleModuleHashRequest(WorldPacket &recvPacket);

        static const std::vector<uint8> RSAPrivatePower;
        static const std::vector<uint8> RSAPrivateModulus;

    private:
        WorldSession* session_;
        WardenRC4 crypt_;
        std::unique_ptr<WardenModule> module_;
};