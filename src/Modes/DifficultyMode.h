#ifndef MODULE_HARDMODE_MODE_H
#define MODULE_HARDMODE_MODE_H

#include "ScriptMgr.h"

enum DifficultyModes
{
    DIFFICULTY_MODE_SELF_CRAFTED = 0,
    DIFFICULTY_MODE_HARDCORE = 1,
    DIFFICULTY_MODE_SLOWXP = 2,
    DIFFICULTY_MODE_COUNT = 3
};

class DifficultyMode
{
public:
    DifficultyMode(bool canBeTraded, bool canSendOrReceiveMail) : _canBeTraded(canBeTraded), _canSendOrReceiveMail(canSendOrReceiveMail) { }
public:
    virtual bool CanSendAuctionHello(WorldSession const* /*session*/, ObjectGuid /*guid*/, Creature* /*creature*/) { return true; }
    virtual bool CanGuildSendBankList(Guild const* /*guild*/, WorldSession* /*session*/, uint8 /*tabId*/, bool /*sendAllSlots*/) { return true; }
    virtual bool CanEquipItem(Player* /*player*/, uint8 /*slot*/, uint16& /*dest*/, Item* /*pItem*/, bool /*swap*/, bool /*notLoading*/) { return true; }
    virtual bool CanCastItemUseSpell(Player* /*player*/, Item* /*item*/, SpellCastTargets const& /*targets*/, uint8 /*castCount*/, uint32 /*glyphIndex*/) { return true; }
    virtual void OnCreateItem(Player* /*player*/, Item* /*item*/, uint32 /*count*/) { }
    virtual void OnPlayerResurrect(Player* /*player*/, float /*restorePercent*/, bool /*applySickness*/) { }
    virtual void OnPlayerReleasedGhost(Player* /*player*/) { }
    virtual bool CanRepopAtGraveyard(Player* /*player*/) { return true; }
    virtual bool OnBeforeTeleport(Player* /*player*/, uint32 /*mapId*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/) { return true; }
    virtual void OnGiveXP(Player* /*player*/, uint32& /*amount*/, Unit* /*victim*/) { }
    virtual void OnQuestComputeXP(Player* /*player*/, Quest const* /*quest*/, uint32& /*xpValue*/) { }
    virtual void OnAfterConfigLoad(bool /*reload*/) { }

    bool IsTradable() { return _canBeTraded; }
    bool IsMailable() { return _canSendOrReceiveMail; }
protected:
    bool _canBeTraded;
    bool _canSendOrReceiveMail;
};

#endif // MODULE_HARDMODE_MODE_H
