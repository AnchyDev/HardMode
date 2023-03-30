#ifndef MODULE_HARDMODE_MODE_HARDCORE_H
#define MODULE_HARDMODE_MODE_HARDCORE_H

#include "DifficultyMode.h"

class DifficultyModeHardCore : public DifficultyMode
{
public:
    DifficultyModeHardCore();

public:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature) override;
};

#endif // MODULE_HARDMODE_MODE_HARDCORE_H
