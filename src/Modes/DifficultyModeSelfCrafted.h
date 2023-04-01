#ifndef MODULE_HARDMODE_MODE_SELFCRAFTED_H
#define MODULE_HARDMODE_MODE_SELFCRAFTED_H

#include "DifficultyMode.h"

#include <vector>

class DifficultyModeSelfCrafted : public DifficultyMode
{
public:
    DifficultyModeSelfCrafted();

public:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature) override;
    bool CanGuildSendBankList(Guild const* guild, WorldSession* session, uint8 tabId, bool sendAllSlots) override;
    bool CanEquipItem(Player* player, uint8 slot, uint16& dest, Item* pItem, bool swap, bool notLoading) override;
    bool IsItemExcluded(uint32 itemId);
    void OnAfterConfigLoad(bool reload) override;

private:
    std::vector<uint32> excludedItemIds;
};

#endif // MODULE_HARDMODE_MODE_SELFCRAFTED_H
