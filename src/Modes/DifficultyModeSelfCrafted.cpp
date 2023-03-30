#include "DifficultyModeSelfCrafted.h"

#include "Config.h"

bool DifficultyModeSelfCrafted::CanSendAuctionHello(WorldSession const* /*session*/, ObjectGuid /*guid*/, Creature* /*creature*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.EnableSelfCrafted", false))
    {
        return true;
    }

    return false;
}
