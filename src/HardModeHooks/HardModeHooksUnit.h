#ifndef MODULE_HARDMODE_HOOKS_UNIT_H
#define MODULE_HARDMODE_HOOKS_UNIT_H

#include "ScriptMgr.h"

class HardModeHooksUnitScript : UnitScript
{
public:
    HardModeHooksUnitScript() : UnitScript("HardModeHooksUnitScript") { }

private:
    void OnAuraRemove(Unit* unit, AuraApplication* auraApp, AuraRemoveMode mode);
};

#endif // MODULE_HARDMODE_HOOKS_UNIT_H
