#ifndef MODULE_HARDMODE_H
#define MODULE_HARDMODE_H

#include "ScriptMgr.h"

class HardModePlayerScript : PlayerScript
{
public:
    HardModePlayerScript() : PlayerScript("HardModePlayerScript") { }

private:
    void OnLevelChanged(Player* player, uint8 oldLevel) override;
};

class HardModeWorldScript : WorldScript
{
public:
    HardModeWorldScript() : WorldScript("HardModeWorldScript") { }

private:
    void OnAfterConfigLoad(bool reload) override;
};

#endif // MODULE_HARDMODE_H
