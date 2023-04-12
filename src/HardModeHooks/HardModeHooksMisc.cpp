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

    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_INTERACT_AUCTION))
    {
        return true;
    }

    // TODO: Player feedback, you cannot use the auction house.
    return false;
}
