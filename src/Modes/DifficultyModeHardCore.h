#ifndef MODULE_HARDMODE_MODE_HARDCORE_H
#define MODULE_HARDMODE_MODE_HARDCORE_H

#include "DifficultyMode.h"

enum HardModeHardCoreConstants
{
    HARDMODE_SPELL_AURA_HARDCORE = 52670,
    HARDMODE_SPELL_AURA_SHADOWBAN = 45681
};

class DifficultyModeHardCore : public DifficultyMode
{
public:
    DifficultyModeHardCore();

public:
    bool CanJoinLfg(Player* player, uint8 roles, lfg::LfgDungeonSet& dungeons, const std::string& comment) override;
    bool CanGroupInvite(Player* player, Player* targetPlayer) override;
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature) override;
    bool CanGuildSendBankList(Guild const* guild, WorldSession* session, uint8 tabId, bool sendAllSlots) override;
    void OnPlayerResurrect(Player* player, float restorePercent, bool applySickness) override;
    void OnPlayerReleasedGhost(Player* player) override;
    bool CanRepopAtGraveyard(Player* player) override;
    bool OnBeforeTeleport(Player* player, uint32 mapId, float x, float y, float z, float orientation, uint32 options, Unit* target) override;
    void AddEffectsForPlayer(Player* player) override;
    void RemoveEffectsForPlayer(Player* player) override;
    void OnAfterConfigLoad(bool reload) override;
};

#endif // MODULE_HARDMODE_MODE_HARDCORE_H
