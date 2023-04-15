#include "HardModeHooks/HardModeHooksWorld.h"
#include "HardModeHandler.h"

void HardModeHooksWorldScript::OnUpdate(uint32 diff)
{
    sHardModeHandler->GetScheduler()->Update(diff);
}
