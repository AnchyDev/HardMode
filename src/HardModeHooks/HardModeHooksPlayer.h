#ifndef MODULE_HARDMODE_HOOKS_PLAYER_H
#define MODULE_HARDMODE_HOOKS_PLAYER_H

#include "Player.h"
#include "ScriptMgr.h"

class HardModeHooksPlayerScript : PlayerScript
{
public:
    HardModeHooksPlayerScript() : PlayerScript("HardModeHooksPlayerScript") { }

private:
    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override;
    void OnQuestComputeXP(Player* player, Quest const* quest, uint32& xpValue) override;
};

#endif // MODULE_HARDMODE_HOOKS_PLAYER_H
