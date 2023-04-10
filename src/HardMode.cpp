#include "HardMode.h"
#include "HardModeCommands.h"
#include "HardModeHandler.h"
#include "HardModeShrineObject.h"

void HardModeWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        sHardModeHandler->ClearHardModes();
    }

    sHardModeHandler->LoadHardModes();
}

void SC_AddHardModeScripts()
{
    new HardModeWorldScript();
    new HardModeShrineObject();
    new HardModeCommandsScript();
}
