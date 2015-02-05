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

#include "Network/ByteBuffer.h"
#include "Opcodes.h"
 
class WorldPacket : public ByteBuffer
{
    public:
        WorldPacket() : ByteBuffer(0), opcode_(NULL_OPCODE)
        {
        }
 
        explicit WorldPacket(Opcodes opcode, size_t res = 200) : ByteBuffer(res), opcode_(opcode)
        {
        }
 
        WorldPacket(const WorldPacket &packet) : ByteBuffer(packet), opcode_(packet.opcode_)
        {
        }
 
        void Initialize(Opcodes opcode, size_t newres = 200)
        {
            clear();
            storage_.reserve(newres);
            opcode_ = opcode;
        }
 
        Opcodes GetOpcode() const { return opcode_; }
        void SetOpcode(Opcodes opcode) { opcode_ = opcode; }
 
    protected:
        Opcodes opcode_;
};