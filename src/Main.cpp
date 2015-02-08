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

int main(int argc, char* argv[])
{
    std::cout << "[Clientless World of Warcraft]" << std::endl;
    std::cout << " - Version: " << uint32_t(GameVersion[0]) << "." << uint32_t(GameVersion[1]) << " . " << uint32_t(GameVersion[2]) << " (" << GameBuild << ")" << std::endl;
    std::cout << " - OS: " << OS << " Platform: " << Platform << " Locale: " << Locale[0] << Locale[1] << Locale[2] << Locale[3] << std::endl;

    std::shared_ptr<Session> session(new Session());
    
    if (!session->LoadSavedData())
        session->RequestData();

    session->Print();

    AuthSession auth(session);

    if (!auth.Authenticate())
    {
        std::cerr << "Couldn't authenticate!" << std::endl;
        return 1;
    }

    WorldSession world(session);
    world.Enter();

    while (world.GetSocket()->IsConnected())
    {
        std::string cmd;
        std::getline(std::cin, cmd);

        world.HandleConsoleCommand(cmd);
    }

    return 0;
}