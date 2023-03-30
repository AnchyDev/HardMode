#ifndef MODULE_HARDMODE_H
#define MODULE_HARDMODE_H

#include "ChatCommand.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

class HardModeMiscScript : MiscScript
{
public:
    HardModeMiscScript() : MiscScript("HardModeMiscScript") { }

private:
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid /*guid*/, Creature* /*creature*/) override;
};

class HardModeCommandScript : public CommandScript
{
public:
    HardModeCommandScript() : CommandScript("HardModeCommandScript") { }

    ChatCommandTable GetCommands() const override;
    static bool HandleHardModeInfoCommand(ChatHandler* handler, Optional<PlayerIdentifier> target);
    static bool HandleHardModeSetModeCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 mode, uint8 value);
};


#endif // MODULE_HARDMODE_H
