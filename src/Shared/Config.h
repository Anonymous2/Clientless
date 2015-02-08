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

static const std::string GameName = "WoW";
static const uint8_t  GameVersion[3] = { 4, 3, 4 };       // Expansion, Major, Minor
static const uint16_t GameBuild = 15595;
static const std::string Platform = "x86";              // x86 | x64 | PPC
static const std::string OS = "OSX";                    // Win | OSX
static const uint8_t Locale[4] = { 'e', 'n', 'U', 'S' };  // enUS | enGB | frFR | deDE | koKR | zhCN | zhTW | ruRU | esES | esMX | ptBR
static const uint32_t TimeZone = 0x3C;
static const uint32_t IP = 0x0100007F;