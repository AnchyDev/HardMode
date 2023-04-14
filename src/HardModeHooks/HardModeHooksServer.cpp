#include "HardModeHooks/HardModeHooksServer.h"
#include "HardModeHandler.h"
#include "HardModeTypes.h"

#include "Player.h"

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
        resend = HandleWhoOverride(player, packet);
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

bool HardModeHooksServerScript::HandleWhoOverride(Player* player, WorldPacket& packet)
{
    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_HIDE_WHOLIST))
    {
        return true;
    }

    uint32 displayCount = packet.read<uint32>();
    uint32 matchCount = packet.read<uint32>();

    // No matches, don't try read any further.
    if (matchCount == 0 && displayCount == 0)
    {
        return false;
    }
    uint32 unknownZoneId = HARDMODE_AREA_UNKNOWN;

    for (uint32 i = 0; i < displayCount; ++i)
    {
        std::string playerName = packet.read<std::string>();
        packet.read_skip<std::string>(); //GuildName
        packet.read_skip<uint32>(); //PlayerLvl
        packet.read_skip<uint32>(); //PlayerClass
        packet.read_skip<uint32>(); //PlayerRace
        packet.read_skip<uint8>(); //PlayerGender
        uint32 playerZoneId = packet.read<uint32>();

        auto targetPlayer = ObjectAccessor::FindPlayerByName(playerName);

        if (!targetPlayer)
        {
            return true;
        }

        packet.put(packet.rpos() - 4, unknownZoneId);
    }

    // Resend modified packet.
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
