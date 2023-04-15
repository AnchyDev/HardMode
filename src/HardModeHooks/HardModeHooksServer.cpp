#include "HardModeHooks/HardModeHooksServer.h"
#include "HardModeHandler.h"
#include "HardModeTypes.h"

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
        resend = HandleWhoListOverride(player, packet);
        break;

    case SMSG_CONTACT_LIST:
    case SMSG_FRIEND_STATUS:
        resend = HandleFriendsListOverride(player, packet);
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

bool HardModeHooksServerScript::HandleWhoListOverride(Player* player, WorldPacket& packet)
{
    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_HIDE_WHOLIST))
    {
        return false;
    }

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

        auto targetPlayer = ObjectAccessor::FindPlayerByName(playerName);
        if (!targetPlayer)
        {
            return false;
        }

        packet.put(packet.rpos() - 4, static_cast<uint32>(HARDMODE_AREA_UNKNOWN));
    }

    // Resend modified packet.
    return true;
}

bool HardModeHooksServerScript::HandleFriendsListOverride(Player* player, WorldPacket& packet)
{
    auto opCode = packet.GetOpcode();

    switch (opCode)
    {
    case SMSG_FRIEND_STATUS:
        return HandleFriendStatus(player, packet);

    case SMSG_CONTACT_LIST:
        return HandleContactList(player, packet);
    }

    return false;
}

bool HardModeHooksServerScript::HandleFriendStatus(Player* player, WorldPacket& packet)
{
    uint8 status = packet.read<uint8>();
    ObjectGuid targetGuid = ObjectGuid(packet.read<uint64>());

    Player* targetPlayer = ObjectAccessor::FindPlayer(targetGuid);

    if (!targetPlayer || !sHardModeHandler->PlayerHasRestriction(targetPlayer, HARDMODE_RESTRICT_HIDE_FRIENDS))
    {
        return false;
    }

    if (status != FRIEND_ADDED_ONLINE &&
        status != FRIEND_ONLINE)
    {
        return false;
    }

    packet.read_skip<std::string>(); // Friend Note
    packet.read_skip<uint8>(); // Friend status

    uint32 area = packet.read<uint32>();

    packet.put(packet.rpos() - 4, static_cast<uint32>(HARDMODE_AREA_UNKNOWN));

    // Resend modified packet.
    return true;
}

bool HardModeHooksServerScript::HandleContactList(Player* player, WorldPacket& packet)
{
    uint32 flags = packet.read<uint32>();
    uint32 count = packet.read<uint32>();

    if (count < 1)
    {
        return false;
    }

    for (uint32 i = 0; i < count; ++i)
    {
        ObjectGuid targetGuid = ObjectGuid(packet.read<uint64>());
        uint32 targetFlags = packet.read<uint32>();
        packet.read_skip<std::string>(); // target note

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

        Player* targetPlayer = ObjectAccessor::FindPlayer(targetGuid);

        if (!targetPlayer || !sHardModeHandler->PlayerHasRestriction(targetPlayer, HARDMODE_RESTRICT_HIDE_FRIENDS))
        {
            // No restriction for this player, skip.
            continue;
        }

        uint32 targetArea = packet.read<uint32>();
        packet.put(packet.rpos() - 4, static_cast<uint32>(HARDMODE_AREA_UNKNOWN));
    }

    return true;
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
