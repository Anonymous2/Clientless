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

#include "TCPSocket.h"

#ifdef _WIN32
    #include <WS2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>

    #define SD_BOTH SHUT_RDWR
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR (-1)
#endif

TCPSocket::TCPSocket() : socket_(INVALID_SOCKET)
{
#ifdef _WIN32
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#endif
}

TCPSocket::~TCPSocket()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

bool TCPSocket::Connect(std::string address)
{
    size_t pos = address.find(':');
    
    std::string host = address.substr(0, pos);
    std::string port = "3724";
 
    if (pos != std::string::npos)
        port = address.substr(pos+1);

    struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* serverinfo = nullptr; 

    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &serverinfo))
        return false;

    struct addrinfo* tmp = nullptr;

	for (tmp = serverinfo; tmp != nullptr; tmp = tmp->ai_next)
	{
		socket_ = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);

		if (socket_ == INVALID_SOCKET)
            continue;

		if (connect(socket_, tmp->ai_addr, tmp->ai_addrlen) == SOCKET_ERROR)
		{
            Disconnect();
			continue;
		}

		break;
	}

	freeaddrinfo(serverinfo);
    return socket_ != INVALID_SOCKET;
}

void TCPSocket::Disconnect()
{
    if (socket_ != INVALID_SOCKET)
    {
        shutdown(socket_, SD_BOTH);

#ifdef _WIN32
        closesocket(socket_);
#else
        close(socket_);
#endif

        socket_ = INVALID_SOCKET;
    }
}

bool TCPSocket::IsConnected()
{
    return socket_ != INVALID_SOCKET;
}

int32 TCPSocket::Read(char* buffer, uint32 length)
{
    int32 result = recv(socket_, buffer, length, MSG_WAITALL);

    if (!result)
    {
        Disconnect();
        return 0;
    }

    if (result == SOCKET_ERROR)
    {
        Disconnect();
        return 0;
    }

    assert(result == length);

    return result;
}

int32 TCPSocket::Read(ByteBuffer* buffer, uint32 length)
{
    char* tmp = new char[length];

    int32 result = Read(tmp, length);
    buffer->append(tmp, length);

    delete[] tmp;
    return result;
}

int32 TCPSocket::Send(uint8 const* buffer, uint32 length)
{
    int32 result = send(socket_, reinterpret_cast<char const*>(buffer), length, 0);

    if (!result)
    {
        Disconnect();
        return 0;
    }

    if (result == SOCKET_ERROR)
    {
        Disconnect();
        return 0;
    }

    assert(result == length);

    return result;
}