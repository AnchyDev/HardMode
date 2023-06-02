#ifndef MODULE_HARDMODE_HOOKS_MISC_H
#define MODULE_HARDMODE_HOOKS_MISC_H

#include "ScriptMgr.h"

class HardModeHooksMiscScript : MiscScript
{
public:
    HardModeHooksMiscScript() : MiscScript("HardModeHooksMiscScript") { }

private:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature) override;
};

#endif // MODULE_HARDMODE_HOOKS_MISC_H
