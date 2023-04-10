#ifndef MODULE_HARDMODE_COMMANDS_H
#define MODULE_HARDMODE_COMMANDS_H

#include "ChatCommand.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

class HardModeCommandsScript : public CommandScript
{
public:
    HardModeCommandsScript() : CommandScript("HardModeCommandsScript") { }

    ChatCommandTable GetCommands() const override;
    static bool HandleHardModeInfoCommand(ChatHandler* handler, Optional<PlayerIdentifier> target);
    static bool HandleHardModeSetModeCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 mode, uint8 value);
    static bool HandleHardModeSetTaintCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 value);
    static bool HandleHardModeSetShadowBanCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 value);
};


#endif // MODULE_HARDMODE_COMMANDS_H
