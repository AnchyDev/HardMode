#include "HardModeHooks/HardModeHooksPlayer.h"
#include "HardModeHandler.h"
#include "HardModeTypes.h"

#include "Config.h"

void HardModeHooksPlayerScript::OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!player)
    {
        return;
    }

    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_RETAIL_XP))
    {
        return;
    }

    amount = (amount / sConfigMgr->GetOption<float>("Rate.XP.Kill", 1));
}

void HardModeHooksPlayerScript::OnQuestComputeXP(Player* player, Quest const* /*quest*/, uint32& xpValue)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!player)
    {
        return;
    }

    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_RETAIL_XP))
    {
        return;
    }

    xpValue = (xpValue / sConfigMgr->GetOption<float>("Rate.XP.Quest", 1));
}
