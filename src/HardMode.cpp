#include "HardMode.h"

#include "Player.h"
#include "Chat.h"

void HardModeUnitScript::OnUnitDeath(Unit* victim, Unit* killer)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!sConfigMgr->GetOption<bool>("HardMode.ExpLossOnDeath", true))
    {
        return;
    }

    if (!victim)
    {
        return;
    }

    if (!victim->ToPlayer())
    {
        return;
    }

    Player* player = victim->ToPlayer();
    player->SetUInt32Value(PLAYER_XP, 0);

    ChatHandler(player->GetSession()).SendSysMessage("You lost all your exp!");
}

void SC_AddHardModeScripts()
{
    new HardModeUnitScript();
}
