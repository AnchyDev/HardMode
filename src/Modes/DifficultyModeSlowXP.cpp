#include "DifficultyModeSlowXP.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"

DifficultyModeSlowXP::DifficultyModeSlowXP() : DifficultyMode(/*canBeTraded*/ true, /*canSendOrReceiveMail*/ true, /*canCrossPlay*/ true) { }

bool DifficultyModeSlowXP::CanGroupInvite(Player* /*player*/, Player* /*targetPlayer*/)
{
    return true;
}

void DifficultyModeSlowXP::OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource)
{
    if (!player)
    {
        return;
    }

    if (xpSource == XPSOURCE_KILL)
    {
        amount = (amount / (sConfigMgr->GetOption<float>("Rate.XP.Kill", 1) * sConfigMgr->GetOption<float>("HardMode.SlowXP.Multiplier", 1)));
    }

    if (xpSource == XPSOURCE_EXPLORE)
    {
        amount = (amount / (sConfigMgr->GetOption<float>("Rate.XP.Explore", 1) * sConfigMgr->GetOption<float>("HardMode.SlowXP.Multiplier", 1)));
    }
}

void DifficultyModeSlowXP::OnQuestComputeXP(Player* player, Quest const* /*quest*/, uint32& xpValue)
{
    if (!player)
    {
        return;
    }

    xpValue = (xpValue / (sConfigMgr->GetOption<float>("Rate.XP.Quest", 1) * sConfigMgr->GetOption<float>("HardMode.SlowXP.Multiplier", 1)));
}

void DifficultyModeSlowXP::OnAfterConfigLoad(bool /*reload*/)
{
    _canBeTraded = sConfigMgr->GetOption<bool>("HardMode.SlowXP.CanBeTraded", true);
    _canSendOrReceiveMail = sConfigMgr->GetOption<bool>("HardMode.SlowXP.CanSendRecMail", true);
    _canCrossPlay = sConfigMgr->GetOption<bool>("HardMode.SlowXP.CanCrossPlay", true);
}
