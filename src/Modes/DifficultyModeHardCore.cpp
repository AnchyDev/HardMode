#include "DifficultyModeHardCore.h"

#include "Config.h"
#include "Player.h"

DifficultyModeHardCore::DifficultyModeHardCore() : DifficultyMode(/*canBeTraded*/ false) { }

bool DifficultyModeHardCore::CanSendAuctionHello(WorldSession const* /*session*/, ObjectGuid /*guid*/, Creature* /*creature*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.EnableHardCore", false))
    {
        return true;
    }

    return false;
}
