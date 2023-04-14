#ifndef MODULE_HARDMODE_HOOKS_SERVER_H
#define MODULE_HARDMODE_HOOKS_SERVER_H

#include "ScriptMgr.h"

enum HardModeServerConstants
{
    HARDMODE_PACKET_TAIL = 696969
};

class HardModeHooksServerScript : ServerScript
{
public:
    HardModeHooksServerScript() : ServerScript("HardModeHooksServerScript") { }

private:
    bool CanPacketSend(WorldSession* session, WorldPacket& packet) override;
    bool HandleWhoOverride(Player* player, WorldPacket& packet);
    bool HasModifiedTail(WorldPacket& packet);
};

#endif // MODULE_HARDMODE_HOOKS_SERVER_H
