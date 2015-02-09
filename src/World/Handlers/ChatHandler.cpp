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

#include "WorldSession.h"

void WorldSession::HandleMessageChat(WorldPacket &recvPacket)
{
    ChatMessage message;
    message.Type = recvPacket.read<ChatType>();
    message.Language = recvPacket.read<Language>();
    recvPacket >> message.SenderGUID;

    if (message.SenderGUID.IsPlayer())
    {
        if (!playerNames_.Has(message.SenderGUID))
            SendNameQuery(message.SenderGUID);
    }

    recvPacket >> message.Flags;

    switch (message.Type)
    {
        case CHAT_MSG_MONSTER_SAY:
        case CHAT_MSG_MONSTER_PARTY:
        case CHAT_MSG_MONSTER_YELL:
        case CHAT_MSG_MONSTER_WHISPER:
        case CHAT_MSG_MONSTER_EMOTE:
        case CHAT_MSG_RAID_BOSS_EMOTE:
        case CHAT_MSG_RAID_BOSS_WHISPER:
        case CHAT_MSG_BATTLENET:
        {
            recvPacket.read_skip<uint32_t>();
            recvPacket >> message.SenderName;
            recvPacket >> message.ReceiverGUID;

            if (!message.ReceiverGUID.IsEmpty() && !message.ReceiverGUID.IsPlayer() && !message.ReceiverGUID.IsPet())
            {
                recvPacket.read_skip<uint32_t>();
                recvPacket >> message.ReceiverName;
            }

            if (message.Language == LANG_ADDON)
                recvPacket >> message.AddonPrefix;

            break;
        }
        case CHAT_MSG_WHISPER_FOREIGN:
        {
            recvPacket.read_skip<uint32_t>();
            recvPacket >> message.SenderName;
            recvPacket >> message.ReceiverGUID;

            if (message.Language == LANG_ADDON)
                recvPacket >> message.AddonPrefix;

            break;
        }
        case CHAT_MSG_BG_SYSTEM_NEUTRAL:
        case CHAT_MSG_BG_SYSTEM_ALLIANCE:
        case CHAT_MSG_BG_SYSTEM_HORDE:
        {
            recvPacket >> message.ReceiverGUID;

            if (!message.ReceiverGUID.IsEmpty() && !message.ReceiverGUID.IsPlayer())
            {
                recvPacket.read_skip<uint32_t>();
                recvPacket >> message.ReceiverName;
            }

            if (message.Language == LANG_ADDON)
                recvPacket >> message.AddonPrefix;

            break;
        }
        case CHAT_MSG_ACHIEVEMENT:
        case CHAT_MSG_GUILD_ACHIEVEMENT:
        {
            recvPacket >> message.ReceiverGUID;

            if (message.Language == LANG_ADDON)
                recvPacket >> message.AddonPrefix;
 
            break;
        }
        default:
        {
            if (recvPacket.GetOpcode() == SMSG_GM_MESSAGECHAT)
            {
                recvPacket.read_skip<uint32_t>();
                recvPacket >> message.SenderName;
            }

            if (message.Type == CHAT_MSG_CHANNEL)
                recvPacket >> message.ChannelName;

            recvPacket >> message.ReceiverGUID;

            if (message.Language == LANG_ADDON)
                recvPacket >> message.AddonPrefix;

            break;
        }
    }

    recvPacket.read_skip<uint32_t>();
    recvPacket >> message.Message;

    ChatTag tag = recvPacket.read<ChatTag>();

    if (message.Type == CHAT_MSG_ACHIEVEMENT || message.Type == CHAT_MSG_GUILD_ACHIEVEMENT)
    {
        uint32_t achievementId;
        recvPacket >> achievementId;
    }
    else if (message.Type == CHAT_MSG_RAID_BOSS_WHISPER || message.Type == CHAT_MSG_RAID_BOSS_EMOTE)
    {
        float displayTime;
        bool hideInChatFrame;

        recvPacket >> displayTime;
        recvPacket >> hideInChatFrame;
    }

    chatMgr_.EnqueueMessage(message);
}