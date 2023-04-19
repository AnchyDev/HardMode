#ifndef MODULE_HARDMODE_HOOKS_GLOBAL_H
#define MODULE_HARDMODE_HOOKS_GLOBAL_H

#include "ScriptMgr.h"

class HardModeHooksGlobalScript : GlobalScript
{
public:
    HardModeHooksGlobalScript() : GlobalScript("HardModeHooksGlobalScript") { }

private:
    void OnBeforeDropAddItem(Player const* player, Loot& loot, bool canRate, uint16 lootMode, LootStoreItem* lootStoreItem, LootStore const& store) override;
};

#endif // MODULE_HARDMODE_HOOKS_GLOBAL_H
