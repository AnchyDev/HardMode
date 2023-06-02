#ifndef MODULE_HARDMODE_SHRINE_OBJ_H
#define MODULE_HARDMODE_SHRINE_OBJ_H

#include "ScriptMgr.h"

enum HardModeShrineConstants
{
    HARDMODE_SHRINE_GREETING = 441101,
    HARDMODE_SHRINE_GREETING_TAINTED = 441102
};

class HardModeShrineObject : GameObjectScript
{
public:
    HardModeShrineObject() : GameObjectScript("HardModeShrineObject") { }

private:
    bool OnGossipHello(Player* player, GameObject* go) override;
    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 mode) override;
};

#endif // MODULE_HARDMODE_SHRINE_OBJ_H
