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

#include "Define.h"

enum AuthCmd : uint8
{
    AUTH_LOGON_CHALLENGE 			= 0x00,
    AUTH_LOGON_PROOF 				= 0x01,
    AUTH_RECONNECT_CHALLENGE 		= 0x02,
    AUTH_RECONNECT_PROOF 			= 0x03,
    REALM_LIST 						= 0x10,
    XFER_INITIATE 					= 0x30,
    XFER_DATA 						= 0x31,
    XFER_ACCEPT 					= 0x32,
    XFER_RESUME 					= 0x33,
    XFER_CANCEL 					= 0x34
};