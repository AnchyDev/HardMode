#include "HardModeHooks/HardModeHooksPlayer.h"
#include "HardModeHandler.h"
#include "HardModeTypes.h"

#include "Config.h"
#include "Player.h"

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

bool HardModeHooksPlayerScript::CanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /*swap*/, bool /*notLoading*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (!player)
    {
        return true;
    }

    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_SELFCRAFTED))
    {
        return true;
    }

    auto itemProto = pItem->GetTemplate();

    // Exclude item ids in the `hardmode_selfcraft_exclude` table.
    if (sHardModeHandler->IsSelfCraftItemExcluded(itemProto->ItemId))
    {
        return true;
    }

    // Allow quest items to be equipped.
    if (itemProto->Class == ITEM_CLASS_QUEST)
    {
        return true;
    }

    // If the item is not creator by the player, block equip.
    if (pItem->GetGuidValue(ITEM_FIELD_CREATOR) != player->GetGUID())
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player, HARDMODE_RESTRICT_SELFCRAFTED);
        std::string alert = Acore::StringFormatFmt("You cannot equip this item while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);

        return false;
    }

    return true;
}

bool HardModeHooksPlayerScript::CanCastItemUseSpell(Player* player, Item* item, SpellCastTargets const& /*targets*/, uint8 /*castCount*/, uint32 /*glyphIndex*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (!player)
    {
        return true;
    }

    if (!sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_SELFCRAFTED))
    {
        return true;
    }

    // Exclude spell ids in the `hardmode_selfcraft_exclude` table.
    if (sHardModeHandler->IsSelfCraftSpellExcluded(item->GetTemplate()->Spells[0].SpellId))
    {
        return true;
    }

    auto itemProto = item->GetTemplate();

    // Only consider blocking on consumables.
    if (itemProto->Class != ITEM_CLASS_CONSUMABLE)
    {
        return true;
    }

    if (itemProto->SubClass != ITEM_SUBCLASS_FOOD &&
        itemProto->SubClass != ITEM_SUBCLASS_POTION &&
        itemProto->SubClass != ITEM_SUBCLASS_ELIXIR &&
        itemProto->SubClass != ITEM_SUBCLASS_FLASK)
    {
        return true;
    }

    if (item->GetGuidValue(ITEM_FIELD_CREATOR) != player->GetGUID())
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player, HARDMODE_RESTRICT_SELFCRAFTED);
        std::string alert = Acore::StringFormatFmt("You cannot use this item while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);

        return false;
    }

    return true;
}

void HardModeHooksPlayerScript::OnPlayerResurrect(Player* player, float /*restorePercent*/, bool /*applySickness*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_PERMADEATH))
    {
        sHardModeHandler->TryShadowBanPlayer(player);
    }
}

void HardModeHooksPlayerScript::OnPlayerReleasedGhost(Player* player)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_PERMADEATH))
    {
        sHardModeHandler->TryShadowBanPlayer(player);
    }
}

bool HardModeHooksPlayerScript::CanRepopAtGraveyard(Player* player)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (!player)
    {
        return true;
    }

    if (sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_PERMADEATH))
    {
        return false;
    }

    return true;
}

bool HardModeHooksPlayerScript::OnBeforeTeleport(Player* player, uint32 mapId, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (sHardModeHandler->IsPlayerShadowBanned(player))
    {
        return (mapId == HARDMODE_AREA_AZSHARACRATER); // Only allow teleports for Shadowban players if it's to the azshara crater / shadow tomb.
    }

    return true;
}

bool HardModeHooksPlayerScript::CanInitTrade(Player* player, Player* target)
{
    if (!player || !target)
    {
        return true;
    }

    if (sHardModeHandler->PlayerHasRestriction(player, HARDMODE_RESTRICT_INTERACT_TRADE))
    {
        player->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);
        target->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);

        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player, HARDMODE_RESTRICT_INTERACT_TRADE);
        std::string alert = Acore::StringFormatFmt("You cannot trade players while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);

        return false;
    }

    if (sHardModeHandler->PlayerHasRestriction(target, HARDMODE_RESTRICT_INTERACT_TRADE))
    {
        player->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);
        target->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);

        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(target, HARDMODE_RESTRICT_INTERACT_TRADE);
        std::string alert = Acore::StringFormatFmt("You cannot trade players in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);

        return false;
    }

    return true;
}

void HardModeHooksPlayerScript::OnLogin(Player* player)
{
    sHardModeHandler->ValidatePlayerAuras(player);
}
