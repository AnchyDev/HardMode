#include "HardModeHooks/HardModeHooksWorld.h"
#include "HardModeHandler.h"

void HardModeHooksWorldScript::OnUpdate(uint32 diff)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    sHardModeHandler->GetScheduler()->Update(diff);
}
