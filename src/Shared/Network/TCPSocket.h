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
#include "ByteBuffer.h"

#ifdef _WIN32
    #include <WinSock2.h>
#else
    typedef unsigned int SOCKET;
#endif

class TCPSocket
{
    public:
        TCPSocket();
        ~TCPSocket();

        virtual bool Connect(std::string address);
        virtual void Disconnect();
        bool IsConnected();

        int32 Read(char* buffer, uint32 length);
        int32 Read(ByteBuffer* buffer, uint32 length);
        int32 Send(uint8 const* buffer, uint32 length);

    private:
        SOCKET socket_;
};