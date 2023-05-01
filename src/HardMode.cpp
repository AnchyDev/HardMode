#include "HardMode.h"
#include "HardModeCommands.h"
#include "HardModeHandler.h"
#include "HardModeShrineObject.h"
#include "HardModeTypes.h"

#include "HardModeHooks/HardModeHooksGlobal.h"
#include "HardModeHooks/HardModeHooksGuild.h"
#include "HardModeHooks/HardModeHooksMisc.h"
#include "HardModeHooks/HardModeHooksPlayer.h"
#include "HardModeHooks/HardModeHooksServer.h"
#include "HardModeHooks/HardModeHooksUnit.h"
#include "HardModeHooks/HardModeHooksWorld.h"

void HardModePlayerScript::OnLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!player)
    {
        return;
    }

    uint32 level = player->GetLevel();

    auto modes = sHardModeHandler->GetHardModes();
    for (auto it = modes->begin(); it != modes->end(); ++it)
    {
        auto mode = it->second;

        if (!sHardModeHandler->IsModeEnabledForPlayer(player->GetGUID(), mode.Id))
        {
            continue;
        }

        auto rewards = sHardModeHandler->GetRewardsForMode(mode.Id);

        if (!rewards || rewards->size() < 1)
        {
            continue;
        }

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

void HardModePlayerScript::OnLogout(Player* player)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    auto guid = player->GetGUID();
    if (auto setting = sHardModeHandler->GetPlayerSetting(guid))
    {
        sHardModeHandler->SavePlayerSetting(player->GetGUID().GetRawValue(), setting);
    }
}

void HardModeWorldScript::OnAfterConfigLoad(bool reload)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (reload)
    {
        sHardModeHandler->SavePlayerSettings();

        sHardModeHandler->ClearHardModes();
        sHardModeHandler->ClearPlayerSettings();
        sHardModeHandler->ClearSelfCraftExcludeIds();
        sHardModeHandler->ClearRewards();
        sHardModeHandler->ClearAuras();
    }

    sHardModeHandler->LoadHardModes();
    sHardModeHandler->LoadPlayerSettings();
    sHardModeHandler->LoadSelfCraftExcludeIds();
    sHardModeHandler->LoadRewards();
    sHardModeHandler->LoadAuras();
}

void HardModeWorldScript::OnShutdown()
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    sHardModeHandler->SavePlayerSettings();
}

void SC_AddHardModeScripts()
{
    new HardModePlayerScript();
    new HardModeWorldScript();
    new HardModeShrineObject();
    new HardModeCommandsScript();

    new HardModeHooksGlobalScript();
    new HardModeHooksGuildScript();
    new HardModeHooksMiscScript();
    new HardModeHooksPlayerScript();
    new HardModeHooksServerScript();
    new HardModeHooksUnitScript();
    new HardModeHooksWorldScript();
}
