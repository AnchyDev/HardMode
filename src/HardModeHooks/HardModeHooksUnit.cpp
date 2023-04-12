#include "HardModeHooks/HardModeHooksUnit.h"
#include "HardModeHandler.h"

void HardModeHooksUnitScript::OnAuraRemove(Unit* unit, AuraApplication* aurApp, AuraRemoveMode mode)
{
    if (!unit)
    {
        return;
    }

    if (!unit->IsPlayer())
    {
        return;
    }

    // Don't reapply aura on death, resurrection handles reapplication already.
    if (mode == AURA_REMOVE_BY_DEATH)
    {
        return;
    }

    Player* player = unit->ToPlayer();
    sHardModeHandler->ValidatePlayerAuras(player);
}
