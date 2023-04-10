#include "HardMode.h"
#include "HardModeHandler.h"

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
}
