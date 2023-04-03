#include "DifficultyModeSlowXP.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"

DifficultyModeSlowXP::DifficultyModeSlowXP() : DifficultyMode(/*canBeTraded*/ true, /*canSendOrReceiveMail*/ true, /*canBeAttacked*/ true) { }

bool DifficultyModeSlowXP::CanGroupInvite(Player* /*player*/, Player* /*targetPlayer*/)
{
    return true;
}

void DifficultyModeSlowXP::OnGiveXP(Player* player, uint32& amount, Unit* victim)
{
    if (!player)
    {
        return;
    }
    if (!victim)
    {
        return;
    }

    amount = (amount / (sConfigMgr->GetOption<float>("Rate.XP.Kill", 1) * sConfigMgr->GetOption<float>("HardMode.SlowXPMultiplier", 1)));
}

void DifficultyModeSlowXP::OnQuestComputeXP(Player* player, Quest const* /*quest*/, uint32& xpValue)
{
    if (!player)
    {
        return;
    }

    xpValue = (xpValue / (sConfigMgr->GetOption<float>("Rate.XP.Quest", 1) * sConfigMgr->GetOption<float>("HardMode.SlowXPMultiplier", 1)));
}
