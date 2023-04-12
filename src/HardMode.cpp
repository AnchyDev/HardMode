#include "HardMode.h"
#include "HardModeCommands.h"
#include "HardModeHandler.h"
#include "HardModeShrineObject.h"
#include "HardModeTypes.h"

#include "HardModeHooks/HardModeHooksGuild.h"
#include "HardModeHooks/HardModeHooksMisc.h"
#include "HardModeHooks/HardModeHooksPlayer.h"

void HardModePlayerScript::OnLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    if (!player)
    {
        return;
    }

    uint32 level = player->GetLevel();

    auto modes = sHardModeHandler->GetHardModes();
    for (auto it = modes->begin(); it != modes->end(); ++it)
    {
        auto mode = it->second;

        if (!sHardModeHandler->IsModeEnabledForPlayer(player, mode.Id))
        {
            continue;
        }

        auto rewards = sHardModeHandler->GetRewardsForMode(mode.Id);

        std::vector<HardModeReward> newRewards;
        bool hasItemRewards = false;
        for (auto reward = rewards->begin(); reward != rewards->end(); ++reward)
        {
            if (level == reward->Level)
            {
                newRewards.push_back(*reward);

                if (reward->Type == HARDMODE_REWARD_TYPE_ITEM)
                {
                    hasItemRewards = true;
                }
            }
        }

        if (newRewards.size() > 0)
        {
            sHardModeHandler->TryRewardPlayer(player, newRewards);

            if (hasItemRewards)
            {
                sHardModeHandler->SendAlert(player, "Congratulations for reaching max level, your rewards are in your mailbox.");
            }
        }
    }
}

void HardModeWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        sHardModeHandler->ClearHardModes();
        sHardModeHandler->ClearSelfCraftExcludeIds();
        sHardModeHandler->ClearRewards();
    }

    sHardModeHandler->LoadHardModes();
    sHardModeHandler->LoadSelfCraftExcludeIds();
    sHardModeHandler->LoadRewards();
}

void SC_AddHardModeScripts()
{
    new HardModePlayerScript();
    new HardModeWorldScript();
    new HardModeShrineObject();
    new HardModeCommandsScript();

    new HardModeHooksGuildScript();
    new HardModeHooksMiscScript();
    new HardModeHooksPlayerScript();
}
