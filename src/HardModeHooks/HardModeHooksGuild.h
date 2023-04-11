#ifndef MODULE_HARDMODE_HOOKS_GUILD_H
#define MODULE_HARDMODE_HOOKS_GUILD_H

#include "ScriptMgr.h"

class HardModeHooksGuildScript : GuildScript
{
public:
    HardModeHooksGuildScript() : GuildScript("HardModeHooksGuildScript") { }

private:
    bool CanGuildSendBankList(Guild const* guild, WorldSession* session, uint8 tabId, bool sendAllSlots) override;
};

#endif // MODULE_HARDMODE_HOOKS_GUILD_H
