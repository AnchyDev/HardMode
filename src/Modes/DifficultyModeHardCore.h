#ifndef MODULE_HARDMODE_MODE_HARDCORE_H
#define MODULE_HARDMODE_MODE_HARDCORE_H

#include "DifficultyMode.h"

class DifficultyModeHardCore : public DifficultyMode
{
public:
    DifficultyModeHardCore();

public:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature) override;
    bool CanGuildSendBankList(Guild const* guild, WorldSession* session, uint8 tabId, bool sendAllSlots) override;
    void OnPlayerResurrect(Player* player, float restorePercent, bool applySickness) override;
    void OnPlayerReleasedGhost(Player* player) override;
    bool CanRepopAtGraveyard(Player* player) override;
    bool OnBeforeTeleport(Player* player, uint32 mapId, float x, float y, float z, float orientation, uint32 options, Unit* target) override;
};

#endif // MODULE_HARDMODE_MODE_HARDCORE_H
