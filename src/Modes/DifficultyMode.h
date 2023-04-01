#ifndef MODULE_HARDMODE_MODE_H
#define MODULE_HARDMODE_MODE_H

#include "ScriptMgr.h"

enum DifficultyModes
{
    DIFFICULTY_MODE_SELF_CRAFTED = 0,
    DIFFICULTY_MODE_HARDCORE = 1,
    DIFFICULTY_MODE_COUNT = 2
};

class DifficultyMode
{
public:
    DifficultyMode(bool canBeTraded, bool canSendOrReceiveMail) : _canBeTraded(canBeTraded), _canSendOrReceiveMail(canSendOrReceiveMail) { }
public:
    virtual bool CanSendAuctionHello(WorldSession const* /*session*/, ObjectGuid /*guid*/, Creature* /*creature*/) { return true; }
    virtual bool CanGuildSendBankList(Guild const* /*guild*/, WorldSession* /*session*/, uint8 /*tabId*/, bool /*sendAllSlots*/) { return true; }
    virtual bool CanEquipItem(Player* /*player*/, uint8 /*slot*/, uint16& /*dest*/, Item* /*pItem*/, bool /*swap*/, bool /*notLoading*/) { return true; }
    virtual void OnPlayerResurrect(Player* /*player*/, float /*restorePercent*/, bool /*applySickness*/) { }
    virtual void OnPlayerReleasedGhost(Player* /*player*/) { }
    virtual bool CanRepopAtGraveyard(Player* /*player*/) { return true; }
    virtual bool OnBeforeTeleport(Player* /*player*/, uint32 /*mapId*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/) { return true; }
    virtual void OnAfterConfigLoad(bool /*reload*/) { }

    bool IsTradable() { return _canBeTraded; }
    bool IsMailable() { return _canSendOrReceiveMail; }
protected:
    bool _canBeTraded;
    bool _canSendOrReceiveMail;
};

#endif // MODULE_HARDMODE_MODE_H
