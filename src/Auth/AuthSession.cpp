/*
 * Copyright (C) 2015 Dehravor <dehravor@gmail.com>
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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

#include "AuthSession.h"
#include "Config.h"
#include "RealmList.h"
#include <algorithm>

AuthSession::AuthSession(std::shared_ptr<Session> session) : session_(session)
{
}

AuthSession::~AuthSession()
{
    socket_.Disconnect();
}

bool AuthSession::Authenticate()
{
    if (!socket_.Connect(session_->GetAuthenticationServerAddress()))
    {
        print("%s", "Couldn't connect to authserver.");
        return false;
    }
    
    bool result = SendLogonChallenge();

    socket_.Disconnect();
    return result;
}

void AuthSession::SendPacket(ByteBuffer& buffer)
{
    if (buffer.empty())
        return;

    socket_.Send(buffer.contents(), buffer.size());
}

bool AuthSession::SendLogonChallenge()
{
    ByteBuffer packet;
    packet << uint8(AUTH_LOGON_CHALLENGE);
    packet << uint8(8);
    packet << uint16(session_->GetAccountName().length() + 30);
    packet << GameName;
    packet << uint8(GameVersion[0]);
    packet << uint8(GameVersion[1]);
    packet << uint8(GameVersion[2]);
    packet << uint16(GameBuild);
    packet << std::string(Platform.rbegin(), Platform.rend());
    packet << std::string(OS.rbegin(), OS.rend());
    packet << uint8(Locale[3]);
    packet << uint8(Locale[2]);
    packet << uint8(Locale[1]);
    packet << uint8(Locale[0]);
    packet << uint32(TimeZone);
    packet << uint32(IP);
    packet << uint8(session_->GetAccountName().length());
    packet.append(session_->GetAccountName().c_str(), session_->GetAccountName().length());

    SendPacket(packet);
    return HandleLogonChallengeResponse();
}

#pragma pack(push, 1)

struct LogonChallengeResponse_Header
{
    AuthCmd Opcode;
    uint8 Unk;
    AuthResult Result;
};
 
struct LogonChallengeResponse_Body
{
    uint8 B[32];
    uint8 g_length;
    uint8 g[1];
    uint8 N_length;
    uint8 N[32];
    uint8 Salt[32];
    uint8 CRCSalt[16];
    uint8 SecurityFlags;
};

#pragma pack(pop)

bool AuthSession::HandleLogonChallengeResponse()
{
    LogonChallengeResponse_Header header;
    socket_.Read((char*)&header, sizeof(LogonChallengeResponse_Header));

    if (header.Result != WOW_SUCCESS)
    {
        print("%s", "[Authentication failed!]");
        print("%s", AuthResultToStr(header.Result).c_str());
        return false;
    }

    LogonChallengeResponse_Body body;
    socket_.Read((char*)&body, sizeof(LogonChallengeResponse_Body));

    // TODO: Implement
    assert(body.SecurityFlags == 0);

    srp6_.Reset();
    srp6_.SetCredentials(session_->GetAccountName(), session_->GetAccountPassword());
    srp6_.SetServerModulus(body.N, body.N_length);
    srp6_.SetServerGenerator(body.g, body.g_length);
    srp6_.SetServerEphemeralB(body.B, 32);
    srp6_.SetServerSalt(body.Salt, 32);
    srp6_.Calculate();

    session_->SetKey(*srp6_.GetClientK());

    return SendLogonProof();
}

bool AuthSession::SendLogonProof()
{
    // TODO: Implement CRC calculation
    BigNumber crc;
    crc.SetRandom(20 * 8);

    ByteBuffer packet;
    packet << uint8(AUTH_LOGON_PROOF);
    packet.append(srp6_.GetClientEphemeralA()->AsByteArray(32).get(), srp6_.GetClientEphemeralA()->GetNumBytes());
    packet.append(srp6_.GetClientM1()->AsByteArray(20).get(), srp6_.GetClientM1()->GetNumBytes());
    packet.append(crc.AsByteArray(20).get(), crc.GetNumBytes());
    packet << uint8(0);
    packet << uint8(0);

    SendPacket(packet);
    return HandleLogonProofResponse();
}

#pragma pack(push, 1)

struct LogonProofResponse_Header
{
    AuthCmd Opcode;
    AuthResult Result;
};
 
struct LogonProofResponse_Body
{
    uint8   M2[20];
    uint32  Unk1;
    uint32  Unk2;
    uint16  Unk3;
};
 
#pragma pack(pop)

bool AuthSession::HandleLogonProofResponse()
{
    LogonProofResponse_Header header;
    socket_.Read((char*)&header, sizeof(LogonProofResponse_Header));

    if (header.Result != WOW_SUCCESS)
    {
        print("%s", "[Authentication failed!]");
        print("%s", AuthResultToStr(header.Result).c_str());
        return false;
    }

    LogonProofResponse_Body body;
    socket_.Read((char*)&body, sizeof(LogonProofResponse_Body));

    if (!srp6_.IsValidM2(body.M2, 20))
        return false;

    return SendRealmlistRequest();
}

bool AuthSession::SendRealmlistRequest()
{
    ByteBuffer packet;
    packet << uint8(REALM_LIST);
    packet << uint32(0x1000);
    SendPacket(packet);
    return HandleRealmlistResponse();
}

#pragma pack(push, 1)

struct RealmlistResponse_Header
{
    AuthCmd Opcode;
    uint16 Length;
    uint32 Unk;
    uint16 Count;
};

#pragma pack(pop)

bool AuthSession::HandleRealmlistResponse()
{
    RealmlistResponse_Header header;
    socket_.Read((char*)&header, sizeof(RealmlistResponse_Header));

    if (!header.Count)
    {
        error("%s", "There are no realms!");
        return false;
    }

    ByteBuffer buffer;
    socket_.Read(&buffer, header.Length - sizeof(header.Unk) - sizeof(header.Count));
    
    RealmList realmlist;
    realmlist.Populate(header.Count, buffer);
    realmlist.Print();

    if (Realm const* realm = realmlist.GetRealmByName(session_->GetRealmName()))
    {
        if (realm->Flags & REALM_FLAG_OFFLINE)
        {
            error("%s", "The selected realm is offline!");
            return false;
        }

        session_->SetRealm(*realm);
    }
    else
    {
        error("There is no realm named '%s'! Please choose another one!", session_->GetRealmName().c_str());
        return false;
    }

    return true;
}

std::string AuthSession::AuthResultToStr(AuthResult result)
{
    switch (result)
    {
        case WOW_SUCCESS:
            return "Success!";
        case WOW_FAIL_BANNED:
            return "This World of Warcraft account has been closed and is no longer available for use. Please check your server's website for further information.";
        case WOW_FAIL_UNKNOWN_ACCOUNT:
        case WOW_FAIL_INCORRECT_PASSWORD:
            return "The information you have entered is not valid. Please check the spelling of the account name and password. If you need help in retrieving a lost or stolen password, see your server's website for more information.";
        case WOW_FAIL_ALREADY_ONLINE :
            return "This account is already logged into World of Warcraft. Please check the spelling and try again.";
        case WOW_FAIL_NO_TIME:
            return "You have used up your prepaid time for this account. Please purchase more to continue playing.";
            break;
        case WOW_FAIL_DB_BUSY:
            return "Could not log in to World of Warcraft at this time. Please try again later.";
            break;
        case WOW_FAIL_VERSION_INVALID:
            return "Unable to validate game version. This may be caused by file corruption or interference of another program. Please visit your server's website for more information and possible solutions to this issue.";
            break;
        case WOW_FAIL_VERSION_UPDATE:
            return "Downloading...";
            break;
        case WOW_FAIL_SUSPENDED:
            return "This World of Warcraft account has been temporarily suspended. Please visit your server's website for further information.";
            break;
        case WOW_SUCCESS_SURVEY:
            return "Connected.";
        case WOW_FAIL_PARENTCONTROL:
            return "Access to this account has been blocked by parental controls. Your settings may be changed in your account preferences at your server's website."; 
        default:
            assert(false);
            return "<Unknown>";
    }
}