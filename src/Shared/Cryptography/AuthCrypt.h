/**
 * mangos-one is a full featured server for World of Warcraft in its first
 * expansion version, supporting clients for patch 2.4.3.
 *
 * Copyright (C) 2005-2013  MaNGOS project <http://getmangos.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#ifndef _AUTHCRYPT_H
#define _AUTHCRYPT_H

#include "Common.h"
#include <vector>
#include <cstring>

class BigNumber;

/**
 * @brief
 *
 */
class AuthCrypt
{
    public:
        /**
         * @brief
         *
         */
        AuthCrypt();
        /**
         * @brief
         *
         */
        ~AuthCrypt();


        /**
         * @brief
         *
         */
        void Init(const BigNumber* K);
        /**
         * @brief
         *
         * @param
         * @param size_t
         */
        void DecryptRecv(uint8_t*, size_t);
        /**
         * @brief
         *
         * @param
         * @param size_t
         */
        void EncryptSend(uint8_t*, size_t);

        /**
         * @brief
         *
         * @return bool
         */
        bool IsInitialized() { return _initialized; }

    private:
        std::vector<uint8_t> _key; /**< TODO */
        uint8_t _send_i, _send_j, _recv_i, _recv_j; /**< TODO */
        bool _initialized; /**< TODO */
};
#endif