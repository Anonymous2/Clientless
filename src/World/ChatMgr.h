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
#include "Common.h"
#include "SharedDefines.h"
#include "ObjectGuid.h"
#include <queue>
#include <mutex>

enum ChatType : uint8_t
{
    CHAT_MSG_ADDON                              = 0xFF,
    CHAT_MSG_SYSTEM                             = 0x00,
    CHAT_MSG_SAY                                = 0x01,
    CHAT_MSG_PARTY                              = 0x02,
    CHAT_MSG_RAID                               = 0x03,
    CHAT_MSG_GUILD                              = 0x04,
    CHAT_MSG_OFFICER                            = 0x05,
    CHAT_MSG_YELL                               = 0x06,
    CHAT_MSG_WHISPER                            = 0x07,
    CHAT_MSG_WHISPER_FOREIGN                    = 0x08,
    CHAT_MSG_WHISPER_INFORM                     = 0x09,
    CHAT_MSG_EMOTE                              = 0x0A,
    CHAT_MSG_TEXT_EMOTE                         = 0x0B,
    CHAT_MSG_MONSTER_SAY                        = 0x0C,
    CHAT_MSG_MONSTER_PARTY                      = 0x0D,
    CHAT_MSG_MONSTER_YELL                       = 0x0E,
    CHAT_MSG_MONSTER_WHISPER                    = 0x0F,
    CHAT_MSG_MONSTER_EMOTE                      = 0x10,
    CHAT_MSG_CHANNEL                            = 0x11,
    CHAT_MSG_CHANNEL_JOIN                       = 0x12,
    CHAT_MSG_CHANNEL_LEAVE                      = 0x13,
    CHAT_MSG_CHANNEL_LIST                       = 0x14,
    CHAT_MSG_CHANNEL_NOTICE                     = 0x15,
    CHAT_MSG_CHANNEL_NOTICE_USER                = 0x16,
    CHAT_MSG_AFK                                = 0x17,
    CHAT_MSG_DND                                = 0x18,
    CHAT_MSG_IGNORED                            = 0x19,
    CHAT_MSG_SKILL                              = 0x1A,
    CHAT_MSG_LOOT                               = 0x1B,
    CHAT_MSG_MONEY                              = 0x1C,
    CHAT_MSG_OPENING                            = 0x1D,
    CHAT_MSG_TRADESKILLS                        = 0x1E,
    CHAT_MSG_PET_INFO                           = 0x1F,
    CHAT_MSG_COMBAT_MISC_INFO                   = 0x20,
    CHAT_MSG_COMBAT_XP_GAIN                     = 0x21,
    CHAT_MSG_COMBAT_HONOR_GAIN                  = 0x22,
    CHAT_MSG_COMBAT_FACTION_CHANGE              = 0x23,
    CHAT_MSG_BG_SYSTEM_NEUTRAL                  = 0x24,
    CHAT_MSG_BG_SYSTEM_ALLIANCE                 = 0x25,
    CHAT_MSG_BG_SYSTEM_HORDE                    = 0x26,
    CHAT_MSG_RAID_LEADER                        = 0x27,
    CHAT_MSG_RAID_WARNING                       = 0x28,
    CHAT_MSG_RAID_BOSS_EMOTE                    = 0x29,
    CHAT_MSG_RAID_BOSS_WHISPER                  = 0x2A,
    CHAT_MSG_FILTERED                           = 0x2B,
    CHAT_MSG_BATTLEGROUND                       = 0x2C,
    CHAT_MSG_BATTLEGROUND_LEADER                = 0x2D,
    CHAT_MSG_RESTRICTED                         = 0x2E,
    CHAT_MSG_BATTLENET                          = 0x2F,
    CHAT_MSG_ACHIEVEMENT                        = 0x30,
    CHAT_MSG_GUILD_ACHIEVEMENT                  = 0x31,
    CHAT_MSG_ARENA_POINTS                       = 0x32,
    CHAT_MSG_PARTY_LEADER                       = 0x33,
    CHAT_MSG_TARGETICONS                        = 0x34,
    CHAT_MSG_BN_WHISPER                         = 0x35,
    CHAT_MSG_BN_WHISPER_INFORM                  = 0x36,
    CHAT_MSG_BN_CONVERSATION                    = 0x37,
    CHAT_MSG_BN_CONVERSATION_NOTICE             = 0x38,
    CHAT_MSG_BN_CONVERSATION_LIST               = 0x39,
    CHAT_MSG_BN_INLINE_TOAST_ALERT              = 0x3A,
    CHAT_MSG_BN_INLINE_TOAST_BROADCAST          = 0x3B,
    CHAT_MSG_BN_INLINE_TOAST_BROADCAST_INFORM   = 0x3C,
    CHAT_MSG_BN_INLINE_TOAST_CONVERSATION       = 0x3D,
    CHAT_MSG_BN_WHISPER_PLAYER_OFFLINE          = 0x3E,
    CHAT_MSG_COMBAT_GUILD_XP_GAIN               = 0x3F,
    CHAT_MSG_CURRENCY                           = 0x40
};

enum ChatTag : uint8_t
{
    CHAT_TAG_NONE   = 0x00,
    CHAT_TAG_AFK    = 0x01,
    CHAT_TAG_DND    = 0x02,
    CHAT_TAG_GM     = 0x04,
    CHAT_TAG_UNK1   = 0x08,
    CHAT_TAG_DEV    = 0x10,
    CHAT_TAG_UNK2   = 0x40,
    CHAT_TAG_COM    = 0x80
};

struct ChatMessage
{
    ChatType Type;
    Languages Language;
    uint32_t Flags;
    ObjectGuid SenderGUID;
    std::string SenderName;
    ObjectGuid ReceiverGUID;
    std::string ReceiverName;
    ChatTag Tag;
    std::string ChannelName;
    std::string AddonPrefix;
    std::string Message;
};

class WorldSession;

class ChatMgr
{
    public:
        ChatMgr(WorldSession* session);

        void EnqueueMessage(const ChatMessage& message);
        void ProcessMessages();

    private:
        std::mutex chatMutex_;
        WorldSession* session_;
        std::queue<std::shared_ptr<ChatMessage>> queue_;
};