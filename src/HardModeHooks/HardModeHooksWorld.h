#ifndef MODULE_HARDMODE_HOOKS_WORLD_H
#define MODULE_HARDMODE_HOOKS_WORLD_H

#include "ScriptMgr.h"

class HardModeHooksWorldScript : WorldScript
{
public:
    HardModeHooksWorldScript() : WorldScript("HardModeHooksWorldScript") { }

private:
    void OnUpdate(uint32 diff) override;
};

#endif // MODULE_HARDMODE_HOOKS_WORLD_H
