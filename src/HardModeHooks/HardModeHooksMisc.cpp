#include "HardModeHooks/HardModeHooksMisc.h"
#include "HardModeHandler.h"

#include "Player.h"

bool HardModeHooksMiscScript::CanSendAuctionHello(WorldSession const* session, ObjectGuid /*guid*/, Creature* /*creature*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    Player* player = session->GetPlayer();
    if (!player)
    {
        return true;
    }

    if (!sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_AUCTION))
    {
        return true;
    }


    auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_AUCTION);
    std::string alert = Acore::StringFormatFmt("You cannot use the auction house in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
    sHardModeHandler->SendAlert(player, alert);

    return false;
}
