#include "HardModeHooks/HardModeHooksPlayer.h"
#include "HardModeHandler.h"
#include "HardModeTypes.h"

#include "Config.h"
#include "Player.h"

void HardModeHooksPlayerScript::OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/, uint8 xpSource)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_RETAIL_XP))
    {
        switch (xpSource)
        {
            case PlayerXPSource::XPSOURCE_KILL:
                amount = (amount / sConfigMgr->GetOption<float>("Rate.XP.Kill", 1));
                break;

            case PlayerXPSource::XPSOURCE_EXPLORE:
                amount = (amount / sConfigMgr->GetOption<float>("Rate.XP.Explore", 1));
                break;
        }
    }

    if (sHardModeHandler->CanTaintPlayer(player->GetGUID()))
    {
        sHardModeHandler->UpdatePlayerTainted(player->GetGUID(), true);
    }
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

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_RETAIL_XP))
    {
        xpValue = (xpValue / sConfigMgr->GetOption<float>("Rate.XP.Quest", 1));
    }
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

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SELFCRAFTED))
    {
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
            auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_SELFCRAFTED);
            std::string alert = Acore::StringFormatFmt("You cannot equip this item while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
            sHardModeHandler->SendAlert(player, alert);

            return false;
        }
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

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SELFCRAFTED))
    {
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
            auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_SELFCRAFTED);
            std::string alert = Acore::StringFormatFmt("You cannot use this item while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
            sHardModeHandler->SendAlert(player, alert);

            return false;
        }
    }

    return true;
}

void HardModeHooksPlayerScript::OnCreateItem(Player* player, Item* item, uint32 /*count*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!player || !item)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SELFCRAFTED))
    {
        if (!sConfigMgr->GetOption<bool>("HardMode.Restrict.SelfCrafted.CreatedBy", true))
        {
            return;
        }

        auto itemProto = item->GetTemplate();

        if (itemProto->Class != ITEM_CLASS_CONSUMABLE)
        {
            return;
        }

        item->SetGuidValue(ITEM_FIELD_CREATOR, player->GetGUID());
    }
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

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_PERMADEATH))
    {
        sHardModeHandler->TryShadowBanPlayer(player->GetGUID());
    }

    sHardModeHandler->ValidatePlayerAuras(player);
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

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_PERMADEATH))
    {
        sHardModeHandler->TryShadowBanPlayer(player->GetGUID());
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

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_PERMADEATH))
    {
        return false;
    }

    return true;
}

bool HardModeHooksPlayerScript::OnBeforeTeleport(Player* player, uint32 mapId, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (sHardModeHandler->IsPlayerShadowBanned(player->GetGUID()))
    {
        return (mapId == HARDMODE_AREA_AZSHARACRATER); // Only allow teleports for Shadowban players if it's to the azshara crater / shadow tomb.
    }

    return true;
}

void HardModeHooksPlayerScript::OnPlayerLearnTalents(Player* player, uint32 /*talentId*/, uint32 /*talentRank*/, uint32 /*spellId*/)
{
    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_TALENTS))
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_TALENTS);
        std::string alert = Acore::StringFormatFmt("You cannot use talent points while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);

        player->resetTalents(true);
    }
}

bool HardModeHooksPlayerScript::CanInitTrade(Player* player, Player* target)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (!player || !target)
    {
        return true;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_TRADE))
    {
        player->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);
        target->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);

        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_TRADE);
        std::string alert = Acore::StringFormatFmt("You cannot trade players while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);

        return false;
    }

    if (sHardModeHandler->PlayerHasRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_TRADE))
    {
        player->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);
        target->GetSession()->SendTradeStatus(TRADE_STATUS_TRADE_CANCELED);

        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_TRADE);
        std::string alert = Acore::StringFormatFmt("You cannot trade players in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);

        return false;
    }

    if (sHardModeHandler->CanTaintPlayer(player->GetGUID()))
    {
        sHardModeHandler->UpdatePlayerTainted(player->GetGUID(), true);
    }

    if (sHardModeHandler->CanTaintPlayer(target->GetGUID()))
    {
        sHardModeHandler->UpdatePlayerTainted(target->GetGUID(), true);
    }

    return true;
}

bool HardModeHooksPlayerScript::CanSendMail(Player* player, ObjectGuid receiverGuid, ObjectGuid /*mailbox*/, std::string& /*subject*/, std::string& /*body*/, uint32 /*money*/, uint32 /*cod*/, Item* /*item*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (!player)
    {
        return true;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_MAIL_SEND))
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_MAIL_SEND);
        std::string alert = Acore::StringFormatFmt("You cannot send mail to other players while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);
        return false;
    }

    if (sHardModeHandler->PlayerHasRestriction(receiverGuid, HARDMODE_RESTRICT_INTERACT_MAIL_RECEIVE))
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(receiverGuid, HARDMODE_RESTRICT_INTERACT_MAIL_RECEIVE);
        std::string alert = Acore::StringFormatFmt("You cannot send mail to players in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);
        return false;
    }

    if (sHardModeHandler->CanTaintPlayer(player->GetGUID()))
    {
        sHardModeHandler->UpdatePlayerTainted(player->GetGUID(), true);
    }

    if (sHardModeHandler->CanTaintPlayer(receiverGuid))
    {
        sHardModeHandler->UpdatePlayerTainted(receiverGuid, true);
    }

    return true;
}

bool HardModeHooksPlayerScript::CanJoinLfg(Player* player, uint8 /*roles*/, lfg::LfgDungeonSet& /*dungeons*/, const std::string& /*comment*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_LFG))
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_LFG);
        std::string alert = Acore::StringFormatFmt("You cannot join looking for group while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);
        return false;
    }

    return true;
}

bool HardModeHooksPlayerScript::CanGroupInvite(Player* player, std::string& memberName)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return true;
    }

    Player* target = ObjectAccessor::FindPlayerByName(memberName);

    if (!target)
    {
        return true;
    }

    if (sHardModeHandler->PlayerHasRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP_CROSSPLAY))
    {
        if (!sHardModeHandler->HasMatchingModesWithRestriction(player, target, HARDMODE_RESTRICT_INTERACT_GROUP_CROSSPLAY))
        {
            auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP_CROSSPLAY);
            std::string alert = Acore::StringFormatFmt("You cannot invite players if you aren't in the cross-play {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
            sHardModeHandler->SendAlert(player, alert);
            return false;
        }
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP_CROSSPLAY))
    {
        if (!sHardModeHandler->HasMatchingModesWithRestriction(player, target, HARDMODE_RESTRICT_INTERACT_GROUP_CROSSPLAY))
        {
            auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP_CROSSPLAY);
            std::string alert = Acore::StringFormatFmt("You cannot invite players that aren't in the cross-play {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
            sHardModeHandler->SendAlert(player, alert);
            return false;
        }
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP))
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP);
        std::string alert = Acore::StringFormatFmt("You cannot invite players while in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);
        return false;
    }

    if (sHardModeHandler->PlayerHasRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP))
    {
        auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP);
        std::string alert = Acore::StringFormatFmt("You cannot invite players in the {} mode(s).", sHardModeHandler->GetDelimitedModes(restrictedModes, ", "));
        sHardModeHandler->SendAlert(player, alert);
        return false;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP_RANGE) ||
        sHardModeHandler->PlayerHasRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP_RANGE))
    {
        uint32 pLevel = player->GetLevel();
        uint32 tLevel = target->GetLevel();

        uint32 range = sConfigMgr->GetOption<uint32>("HardMode.Restrict.Interact.Group.Range", 3);
        uint32 result = std::abs(int32(pLevel - tLevel));
        if (result > range)
        {
            auto restrictedModes = sHardModeHandler->GetPlayerModesFromRestriction(target->GetGUID(), HARDMODE_RESTRICT_INTERACT_GROUP);
            std::string alert = Acore::StringFormatFmt("You cannot invite players who are further than {} levels from you.", range);
            sHardModeHandler->SendAlert(player, alert);
            return false;
        }
    }

    return true;
}

void HardModeHooksPlayerScript::OnLogin(Player* player)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SMALLFISH))
    {
        // Schedule due to issues..
        sHardModeHandler->GetScheduler()->Schedule(1s, [player](TaskContext /*task*/)
        {
            sHardModeHandler->UpdatePlayerScaleSpeed(player, SMALLFISH_SCALE);
        });
    }
    
    sHardModeHandler->ValidatePlayerAuras(player);
}
