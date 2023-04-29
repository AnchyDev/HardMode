#include "HardModeHooks/HardModeHooksGlobal.h"
#include "HardModeHandler.h"

#include "Config.h"
#include "LootMgr.h"

void HardModeHooksGlobalScript::OnBeforeDropAddItem(Player const* player, Loot& /*loot*/, bool /*canRate*/, uint16 /*lootMode*/, LootStoreItem* lootStoreItem, LootStore const& /*store*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    auto ncPlayer = const_cast<Player*>(player);

    if (sHardModeHandler->PlayerHasRestriction(ncPlayer->GetGUID(), HARDMODE_RESTRICT_BAD_LUCK))
    {
        if (!lootStoreItem ||
            !lootStoreItem->itemid)
        {
            return;
        }

        auto itemProto = sObjectMgr->GetItemTemplate(lootStoreItem->itemid);
        if (!itemProto)
        {
            return;
        }

        // Quest items should still drop normally..
        if (lootStoreItem->needs_quest ||
            itemProto->Class == ITEM_CLASS_QUEST)
        {
            return;
        }

        auto roll = urand(0, 100);
        uint32 chance = sConfigMgr->GetOption<uint32>("HardMode.Restrict.BadLuck.Chance", 10);
        if (roll <= chance)
        {
            // If the roll is higher than x percent then it is deleted.
            lootStoreItem->itemid = 0;
        }
    }
}
