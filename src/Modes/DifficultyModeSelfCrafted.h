#ifndef MODULE_HARDMODE_MODE_SELFCRAFTED_H
#define MODULE_HARDMODE_MODE_SELFCRAFTED_H

#include "DifficultyMode.h"

class DifficultyModeSelfCrafted : public DifficultyMode
{
public:
    DifficultyModeSelfCrafted() { }

public:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature) override;
    bool CanEquipItem(Player* player, uint8 slot, uint16& dest, Item* pItem, bool swap, bool notLoading) override;
};

#endif // MODULE_HARDMODE_MODE_SELFCRAFTED_H
