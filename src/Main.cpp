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

#include "Config.h"
#include "Session.h"
#include "Auth/AuthSession.h"
#include "World/WorldSession.h"
#include <iostream>

int main(int argc, char* argv[])
{
    print("%s", "[Clientless World of Warcraft]");
    print(" - Version: %d.%d.%d (%d)", GameVersion[0], GameVersion[1], GameVersion[2], GameBuild);
    print(" - OS: %s Platform: %s Locale: %c%c%c%c", OS.c_str(), Platform.c_str(), Locale[0], Locale[1], Locale[2], Locale[3]);

    std::shared_ptr<Session> session(new Session());
    
    if (!session->LoadSavedData())
        session->RequestData();

    session->Print();

    AuthSession auth(session);

    if (!auth.Authenticate())
    {
        print("%s", "Couldn't authenticate!");
        return 1;
    }

    WorldSession world(session);
    world.Enter();

    while (world.IsConnected())
    {
        std::string cmd;
        std::getline(std::cin, cmd);

        world.HandleConsoleCommand(cmd);
    }

    return 0;
}