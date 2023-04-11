#include "HardMode.h"
#include "HardModeCommands.h"
#include "HardModeHandler.h"
#include "HardModeShrineObject.h"

#include "HardModeHooks/HardModeHooksGuild.h"
#include "HardModeHooks/HardModeHooksMisc.h"
#include "HardModeHooks/HardModeHooksPlayer.h"

void HardModeWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        sHardModeHandler->ClearHardModes();
        sHardModeHandler->ClearSelfCraftExcludeIds();
    }

    sHardModeHandler->LoadHardModes();
    sHardModeHandler->LoadSelfCraftExcludeIds();
}

void SC_AddHardModeScripts()
{
    new HardModeWorldScript();
    new HardModeShrineObject();
    new HardModeCommandsScript();

    new HardModeHooksGuildScript();
    new HardModeHooksMiscScript();
    new HardModeHooksPlayerScript();
}
