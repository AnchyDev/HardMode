#include "HardModeHooks/HardModeHooksGuild.h"
#include "HardModeHandler.h"

bool HardModeHooksGuildScript::CanGuildSendBankList(Guild const* /*guild*/, WorldSession* session, uint8 /*tabId*/, bool /*sendAllSlots*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (!session)
    {
        return true;
    }

    Player* player = session->GetPlayer();
    if (!player)
    {
        return true;
    }

    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_INTERACT_GUILDBANK))
    {
        return true;
    }

    // TODO: Player feedback, you cannot use the guild bank.
    return false;
}
