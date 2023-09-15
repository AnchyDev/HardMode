#include "HardModeHooks/HardModeHooksServer.h"
#include "HardModeHandler.h"
#include "HardModeTypes.h"

#include "Config.h"
#include "Guild.h"
#include "Player.h"
#include "SocialMgr.h"

bool HardModeHooksServerScript::CanPacketSend(WorldSession* session, WorldPacket& packet)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (!session)
    {
        return true;
    }

    auto player = session->GetPlayer();
    if (!player)
    {
        return true;
    }

    // The packet was modified and re-sent, let it through.
    if (HasModifiedTail(packet))
    {
        return true;
    }

    auto opCode = packet.GetOpcode();
    bool resend = false;

    switch (opCode)
    {
    case SMSG_WHO:
        resend = HandleWhoListOverride(packet);
        break;

    case SMSG_FRIEND_STATUS:
    case SMSG_CONTACT_LIST:
        resend = HandleFriendsListOverride(packet);
        break;

    case SMSG_GUILD_ROSTER:
        resend = HandleGuildRosterOverride(packet);
        break;
    case SMSG_INSPECT_TALENT:
        HandleInspectOverride(player, packet);
        break;
    }

    if (resend)
    {
        // Modify the tail, resend and stop sending the original.
        packet << HARDMODE_PACKET_TAIL;
        player->SendDirectMessage(&packet);

        return false;
    }

    return true;
}

bool HardModeHooksServerScript::HandleWhoListOverride(WorldPacket& packet)
{
    bool resendPacket = false;

    uint32 displayCount = packet.read<uint32>();
    uint32 matchCount = packet.read<uint32>();

    // No matches, don't try read any further.
    if (matchCount == 0 && displayCount == 0)
    {
        return false;
    }

    for (uint32 i = 0; i < displayCount; ++i)
    {
        std::string playerName = packet.read<std::string>();
        packet.read_skip<std::string>(); //GuildName
        packet.read_skip<uint32>(); //PlayerLvl
        packet.read_skip<uint32>(); //PlayerClass
        packet.read_skip<uint32>(); //PlayerRace
        packet.read_skip<uint8>(); //PlayerGender
        packet.read_skip<uint32>(); //PlayerZoneId

        auto targetGuid = sHardModeHandler->GetGUIDFromPlayerName(playerName);
        if (targetGuid && sHardModeHandler->PlayerHasRestriction(*targetGuid, HARDMODE_RESTRICT_HIDE_WHOLIST))
        {
            packet.put(packet.rpos() - 4, static_cast<uint32>(HARDMODE_AREA_UNKNOWN)); // ZoneId
            packet.put(packet.rpos() - 17, 0); // PlayerLvl
            resendPacket = true;
        }
    }

    return resendPacket;
}

bool HardModeHooksServerScript::HandleFriendsListOverride(WorldPacket& packet)
{
    auto opCode = packet.GetOpcode();

    switch (opCode)
    {
    case SMSG_FRIEND_STATUS:
        return HandleFriendStatus(packet);

    case SMSG_CONTACT_LIST:
        return HandleContactList(packet);
    }

    return false;
}

bool HardModeHooksServerScript::HandleFriendStatus(WorldPacket& packet)
{
    bool resendPacket = false;

    uint8 status = packet.read<uint8>();
    ObjectGuid targetGuid = ObjectGuid(packet.read<uint64>());

    if (status == FRIEND_REMOVED)
    {
        return false;
    }

    if (status == FRIEND_ADDED_ONLINE ||
        status == FRIEND_ADDED_OFFLINE)
    {
        packet.read_skip<std::string>(); // Friend Note
    }

    if (status == FRIEND_ADDED_ONLINE ||
        status == FRIEND_ONLINE)
    {
        packet.read_skip<uint8>(); // Friend status
        packet.read_skip<uint32>(); // Friend area
        packet.read_skip<uint32>(); // Friend level
    }

    if (targetGuid && sHardModeHandler->PlayerHasRestriction(targetGuid, HARDMODE_RESTRICT_HIDE_FRIENDS))
    {
        packet.put(packet.rpos() - 8, static_cast<uint32>(HARDMODE_AREA_UNKNOWN)); // Area
        packet.put(packet.rpos() - 4, 0); // Level
        resendPacket = true;
    }

    return resendPacket;
}

bool HardModeHooksServerScript::HandleContactList(WorldPacket& packet)
{
    bool resendPacket = false;

    packet.read_skip<uint32>(); // Flags
    uint32 contactCount = packet.read<uint32>();

    if (contactCount < 1)
    {
        return false;
    }

    for (uint32 i = 0; i < contactCount; ++i)
    {
        ObjectGuid targetGuid = ObjectGuid(packet.read<uint64>());
        uint32 targetFlags = packet.read<uint32>();
        std::string targetNote = packet.read<std::string>(); // target note

        if (!(targetFlags & SOCIAL_FLAG_FRIEND))
        {
            // Not a friend query, goto next.
            continue;
        }

        uint8 targetStatus = packet.read<uint8>();
        if (!targetStatus)
        {
            // Target is not online, goto next.
            continue;
        }

        packet.read_skip<uint32>(); // target area
        packet.read_skip<uint32>(); // target level
        packet.read_skip<uint32>(); // target class

        if (targetGuid && sHardModeHandler->PlayerHasRestriction(targetGuid, HARDMODE_RESTRICT_HIDE_FRIENDS))
        {
            packet.put(packet.rpos() - 12, static_cast<uint32>(HARDMODE_AREA_UNKNOWN)); // Area
            packet.put(packet.rpos() - 8, 0); // Level
            resendPacket = true;
        }
    }

    return resendPacket;
}

bool HardModeHooksServerScript::HandleGuildRosterOverride(WorldPacket& packet)
{
    bool resendPacket = false;

    uint32 memberCount = packet.read<uint32>();
    packet.read_skip<std::string>(); // WelcomeText
    packet.read_skip<std::string>(); // InfoText
    uint32 rankCount = packet.read<uint32>();

    for (uint32 i = 0; i < rankCount; ++i)
    {
        packet.read_skip<uint32>(); //RankFlags
        packet.read_skip<uint32>(); //RankWithdrawGoldLimit

        for (uint8 i = 0; i < GUILD_BANK_MAX_TABS; i++)
        {
            packet.read_skip<uint32>(); //RankTabFlags
            packet.read_skip<uint32>(); //RankTabWithdrawItemLimit
        }
    }

    for (uint32 i = 0; i < memberCount; ++i)
    {
        ObjectGuid memberGuid = ObjectGuid(packet.read<uint64>());
        uint8 memberStatus = packet.read<uint8>(); //MemberStatus
        std::string memberName = packet.read<std::string>(); //MemberName
        packet.read_skip<int32>(); //MemberRankId
        packet.read_skip<uint8>(); //MemberLevel
        packet.read_skip<uint8>(); //MemberClassId
        packet.read_skip<uint8>(); //MemberGender
        packet.read_skip<int32>(); //MemberAreaId

        if (memberGuid && sHardModeHandler->PlayerHasRestriction(memberGuid, HARDMODE_RESTRICT_HIDE_GUILD))
        {
            packet.put(packet.rpos() - 4, static_cast<uint32>(HARDMODE_AREA_UNKNOWN)); // Area
            packet.put(packet.rpos() - 7, 0); // Area
            resendPacket = true;
        }

        if (!memberStatus)
        {
            packet.read_skip<float>(); //MemberLastSave
        }

        packet.read_skip<std::string>(); //MemberNote
        packet.read_skip<std::string>(); //MemberOfficerNote
    }

    return resendPacket;
}

void HardModeHooksServerScript::HandleInspectOverride(Player* player, WorldPacket& packet)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Inspect.Alert", true))
    {
        return;
    }

    uint64 packGuid;
    packet.readPackGUID(packGuid);
    ObjectGuid targetGuid = ObjectGuid(packGuid);
    Player* targetPlayer = ObjectAccessor::FindPlayer(targetGuid);

    if (!targetPlayer)
    {
        return;
    }

    auto modes = sHardModeHandler->GetHardModes();
    bool hasModes = false;
    for (auto it = modes->begin(); it != modes->end(); ++it)
    {
        auto mode = it->second;

        if (sHardModeHandler->IsModeEnabledForPlayer(targetPlayer->GetGUID(), mode.Id))
        {
            hasModes = true;

            // No need to iterate everything.
            break;
        }
    }

    if (!hasModes)
    {
        return;
    }

    std::string sFormat = Acore::StringFormatFmt("{} has modes {}.", targetPlayer->GetPlayerName(), sHardModeHandler->GetNamesFromEnabledModes(targetPlayer));
    sHardModeHandler->SendAlert(player, sFormat);
}

bool HardModeHooksServerScript::HasModifiedTail(WorldPacket& packet)
{
    if (packet.size() < 4)
    {
        return false;
    }

    // Read the last 4 bytes of the packet to check for modified tail.
    return packet.read<uint32>(packet.size() - 4) == HARDMODE_PACKET_TAIL;
}
