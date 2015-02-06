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

#include "WorldSocket.h"
#include "WorldSession.h"

#ifndef _WIN32
    #include <netinet/in.h>
#endif

WorldSocket::WorldSocket(WorldSession* session) : session_(session), isRunning_(false)
{
    inflateStream_.zalloc = Z_NULL;
    inflateStream_.zfree = Z_NULL;
    inflateStream_.opaque = Z_NULL;
    inflateInit(&inflateStream_);
}

WorldSocket::~WorldSocket()
{
    Disconnect();

    if (senderThread_.joinable())
        senderThread_.join();

    if (receiverThread_.joinable())
        receiverThread_.join();

    inflateEnd(&inflateStream_);
}

bool WorldSocket::Connect(std::string address)
{
    if (!TCPSocket::Connect(address))
        return false;

    isRunning_ = true;
    senderThread_ = std::thread(&WorldSocket::RunSenderThread, this);
    receiverThread_ = std::thread(&WorldSocket::RunReceiverThread, this);
    return true;
}

void WorldSocket::Disconnect()
{
    isRunning_ = false;

    std::lock_guard<std::mutex> sendLock(sendMutex_);

    while (!sendQueue_.empty())
        sendQueue_.pop();

    std::lock_guard<std::mutex> receiveLock(receiveMutex_);

    while (!receiveQueue_.empty())
        receiveQueue_.pop();

    TCPSocket::Disconnect();
}

void WorldSocket::EnqueuePacket(WorldPacket &packet)
{
    std::shared_ptr<WorldPacket> copy(new WorldPacket(packet));

    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.push(copy);
}

std::shared_ptr<WorldPacket> WorldSocket::GetNextPacket()
{
    if (receiveQueue_.empty())
        return nullptr;

    std::lock_guard<std::mutex> lock(receiveMutex_);
    std::shared_ptr<WorldPacket> packet = receiveQueue_.front();
    receiveQueue_.pop();

    return packet;
}

void WorldSocket::RunSenderThread()
{
    while (isRunning_)
    {
        if (!sendQueue_.empty())
        {
            // Acquire next packet
            std::lock_guard<std::mutex> lock(sendMutex_);
            std::shared_ptr<WorldPacket> packet = sendQueue_.front();
            sendQueue_.pop();

            ByteBuffer prepared;

            uint16 size = htons(packet->size() + 4);
            uint32 opcode = uint32(packet->GetOpcode());

            packetCrypt_.EncryptSend((uint8*)&size, 2);
            packetCrypt_.EncryptSend((uint8*)&opcode, 4);

            prepared << size;
            prepared << opcode;

            if (!packet->empty())
                prepared.append(packet->contents(), packet->size());

            Send(prepared.contents(), prepared.size());

            if (packet->GetOpcode() == CMSG_AUTH_SESSION)
                packetCrypt_.Initialize(&session_->session_->GetKey());
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void WorldSocket::RunReceiverThread()
{
    uint8 header[5];
    uint32 size;
    Opcodes opcode;

    while (isRunning_)
    {
        // Read normal header (4 bytes)
        Read((char*)&header[0], 4);

        if (!IsConnected())
            break;

        packetCrypt_.DecryptReceived(&header[0], 4);

        // Read additional header byte if necessary (1 byte)
        if (header[0] & 0x80)
        {
            Read((char*)&header[4], 1);

            if (!IsConnected())
                break;

            packetCrypt_.DecryptReceived(&header[4], 1);
        }

        // Calculate size and opcode
        if (header[0] & 0x80)
        {
            size = ((header[0] & 0x7F) << 16) | (header[1] << 8) | header[2];
            opcode = static_cast<Opcodes>(header[3] | (header[4] << 8));
        }
        else
        {
            size = (header[0] << 8) | header[1];
            opcode = static_cast<Opcodes>(header[2] | (header[3] << 8));
        }

        size -= sizeof(Opcodes);

        // Read body
        std::shared_ptr<WorldPacket> packet(new WorldPacket(opcode, size));
        packet->resize(size);

        if (size)
            Read(reinterpret_cast<char*>(packet->contents()), size);

        if (!IsConnected())
            break;

        if (packet->GetOpcode() & COMPRESSED_OPCODE_MASK)
            DecompressPacket(packet);

        std::lock_guard<std::mutex> lock(receiveMutex_);
        receiveQueue_.push(packet);
    }

    print("%s", "Disconnected from the server.");
    isRunning_ = false;
}

void WorldSocket::DecompressPacket(std::shared_ptr<WorldPacket> packet)
{
    // Calculate real header
    Opcodes opcode = static_cast<Opcodes>(packet->GetOpcode() ^ COMPRESSED_OPCODE_MASK);

    uint32 size;
    (*packet) >> size;

    // Decompress packet
    std::vector<uint8> decompressedBytes;
    decompressedBytes.resize(size);

    inflateStream_.avail_in = packet->size() - packet->rpos();
    inflateStream_.next_in = const_cast<uint8*>(packet->contents() + packet->rpos());
    inflateStream_.avail_out = size;
    inflateStream_.next_out = &decompressedBytes[0];

    assert(inflate(&inflateStream_, Z_NO_FLUSH) == Z_OK);
    assert(inflateStream_.avail_in == 0);

    packet->Initialize(opcode, size);
    packet->append(&decompressedBytes[0], size);
}
