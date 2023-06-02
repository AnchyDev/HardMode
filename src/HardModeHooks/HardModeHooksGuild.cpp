#include "HardModeHooks/HardModeHooksGuild.h"
#include "HardModeHandler.h"
#include "HardModeTypes.h"

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

    if (!sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_GUILDBANK))
    {
        return true;
    }

    auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_GUILDBANK);
    std::string alert = Acore::StringFormatFmt("You cannot use the guild bank in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
    sHardModeHandler->SendAlert(player, alert);

    return false;
}
