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
#include <cstdint>

enum AuthResult : uint8_t
{
    WOW_SUCCESS                                  = 0x00,
    WOW_FAIL_BANNED                              = 0x03,
    WOW_FAIL_UNKNOWN_ACCOUNT                     = 0x04,
    WOW_FAIL_INCORRECT_PASSWORD                  = 0x05,
    WOW_FAIL_ALREADY_ONLINE                      = 0x06,
    WOW_FAIL_NO_TIME                             = 0x07,
    WOW_FAIL_DB_BUSY                             = 0x08,
    WOW_FAIL_VERSION_INVALID                     = 0x09,
    WOW_FAIL_VERSION_UPDATE                      = 0x0A,
    WOW_FAIL_INVALID_SERVER                      = 0x0B,
    WOW_FAIL_SUSPENDED                           = 0x0C,
    WOW_FAIL_FAIL_NOACCESS                       = 0x0D,
    WOW_SUCCESS_SURVEY                           = 0x0E,
    WOW_FAIL_PARENTCONTROL                       = 0x0F,
    WOW_FAIL_LOCKED_ENFORCED                     = 0x10,
    WOW_FAIL_TRIAL_ENDED                         = 0x11,
    WOW_FAIL_USE_BATTLENET                       = 0x12,
    WOW_FAIL_ANTI_INDULGENCE                     = 0x13,
    WOW_FAIL_EXPIRED                             = 0x14,
    WOW_FAIL_NO_GAME_ACCOUNT                     = 0x15,
    WOW_FAIL_CHARGEBACK                          = 0x16,
    WOW_FAIL_INTERNET_GAME_ROOM_WITHOUT_BNET     = 0x17,
    WOW_FAIL_GAME_ACCOUNT_LOCKED                 = 0x18,
    WOW_FAIL_UNLOCKABLE_LOCK                     = 0x19,
    WOW_FAIL_CONVERSION_REQUIRED                 = 0x20,
    WOW_FAIL_DISCONNECTED                        = 0xFF
};