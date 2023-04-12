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

    bool CanEquipItem(Player* player, uint8 slot, uint16& dest, Item* pItem, bool swap, bool notLoading) override;
    bool CanCastItemUseSpell(Player* player, Item* item, SpellCastTargets const& targets, uint8 castCount, uint32 glyphIndex) override;

    void OnPlayerResurrect(Player* player, float restorePercent, bool applySickness) override;
    void OnPlayerReleasedGhost(Player* player) override;
    bool CanRepopAtGraveyard(Player* player) override;

    bool OnBeforeTeleport(Player* player, uint32 mapId, float x, float y, float z, float orientation, uint32 options, Unit* target) override;

    bool CanInitTrade(Player* player, Player* target) override;

    void OnLogin(Player* player) override;
};

#endif // MODULE_HARDMODE_HOOKS_PLAYER_H
