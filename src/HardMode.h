#ifndef MODULE_HARDMODE_H
#define MODULE_HARDMODE_H

#include "ScriptMgr.h"
#include "Config.h"

class HardModeUnitScript : public UnitScript
{
public:
    HardModeUnitScript() : UnitScript("HardModeUnitScript") { }

private:
    void OnUnitDeath(Unit* victim, Unit* killer) override;
};

#endif // MODULE_HARDMODE_H
