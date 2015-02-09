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

#include "ChatMgr.h"
#include "WorldSession.h"

ChatMgr::ChatMgr(WorldSession* session) : session_(session)
{

}

void ChatMgr::EnqueueMessage(const ChatMessage& message)
{
    std::shared_ptr<ChatMessage> copy(new ChatMessage(message));

    std::lock_guard<std::mutex> lock(chatMutex_);
    queue_.push(copy);
}

void ChatMgr::ProcessMessages()
{
    std::shared_ptr<ChatMessage> message = nullptr;

    // Access queue
    {
        std::lock_guard<std::mutex> lock(chatMutex_);

        if (queue_.empty())
            return;

        message = queue_.front();

        // If sender's name query has not arrived yet
        if (message->SenderGUID.IsPlayer() && message->SenderName.empty())
        {
            const PlayerNameCache* cache = session_->GetPlayerNameCache();

            if (!cache->Has(message->SenderGUID))
                return;

            const PlayerNameEntry* entry = cache->Get(message->SenderGUID);
            message->SenderName = entry->Name;
        }

        // If receiver's name query has not arrived yet
        if (message->ReceiverGUID.IsPlayer() && message->ReceiverName.empty())
        {
            const PlayerNameCache* cache = session_->GetPlayerNameCache();

            if (!cache->Has(message->ReceiverGUID))
                return;

            const PlayerNameEntry* entry = cache->Get(message->ReceiverGUID);
            message->ReceiverName = entry->Name;
        }

        queue_.pop();
    }

    if (!message)
        return;

    switch (message->Tag)
    {
        case CHAT_TAG_AFK:
            message->SenderName = "<Away>" + message->SenderName;
            break;
        case CHAT_TAG_DND:
            message->SenderName = "<Busy>" + message->SenderName;
            break;
        case CHAT_TAG_GM:
            message->SenderName = "<GM>" + message->SenderName;
            break;
        case CHAT_TAG_DEV:
            message->SenderName = "<Dev>" + message->SenderName;
            break;
    }

    message->SenderName = "[" + message->SenderName + "]";

    switch (message->Type)
    {
        case CHAT_MSG_SYSTEM:
            std::cout << message->Message << std::endl;
            break;
        case CHAT_MSG_SAY:
        case CHAT_MSG_MONSTER_SAY:
            std::cout << message->SenderName << " says: " << message->Message << std::endl;
            break;
        case CHAT_MSG_YELL:
        case CHAT_MSG_MONSTER_YELL:
            std::cout << message->SenderName << " yells: " << message->Message << std::endl;
            break;
        case CHAT_MSG_PARTY:
        case CHAT_MSG_MONSTER_PARTY:
            std::cout << "[Party]" << message->SenderName << ": " << message->Message << std::endl;
            break;
        case CHAT_MSG_PARTY_LEADER:
            std::cout << "[Party Leader]" << message->SenderName << ": " << message->Message << std::endl;
            break;
        case CHAT_MSG_RAID:
            std::cout << "[Raid]" << message->SenderName << ": " << message->Message << std::endl;
            break;
        case CHAT_MSG_RAID_WARNING:
            std::cout << "[Raid Warning]" << message->SenderName << ": " << message->Message << std::endl;
            break;
        case CHAT_MSG_GUILD:
            std::cout << "[Guild]" << message->SenderName << ": " << message->Message << std::endl;
            break;
        case CHAT_MSG_OFFICER:
            std::cout << "[Officer]" << message->SenderName << ": " << message->Message << std::endl;
            break;
        case CHAT_MSG_WHISPER:
        case CHAT_MSG_MONSTER_WHISPER:
            std::cout << message->SenderName << " whispers: " << message->Message << std::endl;
            break;
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_MONSTER_EMOTE:
            std::cout << message->SenderName << " " << message->Message << std::endl;
            break;
        case CHAT_MSG_CHANNEL:
            std::cout << "[" << message->ChannelName << "] " << message->SenderName << ": " << message->Message << std::endl;
            break;
    }
}