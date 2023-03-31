#ifndef MODULE_HARDMODE_MODE_HARDCORE_H
#define MODULE_HARDMODE_MODE_HARDCORE_H

#include "DifficultyMode.h"

class DifficultyModeHardCore : public DifficultyMode
{
public:
    DifficultyModeHardCore();

public:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature) override;
    void OnPlayerResurrect(Player* player, float restorePercent, bool applySickness) override;
    void OnPlayerReleasedGhost(Player* player) override;
    bool CanRepopAtGraveyard(Player* player) override;
};

#endif // MODULE_HARDMODE_MODE_HARDCORE_H
