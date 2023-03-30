#ifndef MODULE_HARDMODE_MODE_SELFCRAFTED_H
#define MODULE_HARDMODE_MODE_SELFCRAFTED_H

#include "DifficultyMode.h"

class DifficultyModeSelfCrafted : public DifficultyMode
{
public:
    DifficultyModeSelfCrafted() { }

public:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid /*guid*/, Creature* /*creature*/) override;
};

#endif // MODULE_HARDMODE_MODE_SELFCRAFTED_H
