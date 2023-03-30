#include "DifficultyModeSelfCrafted.h"

#include "Config.h"
#include "Player.h"

DifficultyModeSelfCrafted::DifficultyModeSelfCrafted() : DifficultyMode(/*canBeTraded*/ false) { }

bool DifficultyModeSelfCrafted::CanSendAuctionHello(WorldSession const* /*session*/, ObjectGuid /*guid*/, Creature* /*creature*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.EnableSelfCrafted", false))
    {
        return true;
    }

    return false;
}

bool DifficultyModeSelfCrafted::CanEquipItem(Player* player, uint8 slot, uint16& dest, Item* pItem, bool swap, bool notLoading)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.EnableSelfCrafted", false))
    {
        return true;
    }

    // Allow fishing poles to be equipped since you cannot craft them.
    if (pItem->GetTemplate()->Class == ITEM_CLASS_WEAPON &&
        pItem->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
    {
        return true;
    }

    if (!pItem->GetTemplate()->HasSignature())
    {
        return false;
    }

    if (pItem->GetGuidValue(ITEM_FIELD_CREATOR) != player->GetGUID())
    {
        return false;
    }

    return true;
}
