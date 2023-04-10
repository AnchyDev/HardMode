#include "HardModeShrineObject.h"
#include "HardModeHandler.h"

#include "Config.h"
#include "Player.h"
#include "ScriptedGossip.h"

bool HardModeShrineObject::OnGossipHello(Player* player, GameObject* go)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return false;
    }

    auto hardModes = sHardModeHandler->GetHardModes();

    for (auto it = hardModes->begin(); it != hardModes->end(); ++it)
    {
        auto mode = it->second;

        if (!mode.Enabled)
        {
            continue;
        }

        bool flag = sHardModeHandler->IsModeEnabledForPlayer(player, mode.Id);
        std::string state = flag ? "Disable" : "Enable";
        std::string format = Acore::StringFormatFmt("{} {} mode.", state, mode.Name);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, format, 0, mode.Id);
    }

    SendGossipMenuFor(player, 1, go->GetGUID());

    return true;
}

bool HardModeShrineObject::OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 mode)
{
    bool flag = sHardModeHandler->IsModeEnabledForPlayer(player, mode);
    sHardModeHandler->UpdateModeForPlayer(player, mode, !flag);

    CloseGossipMenuFor(player);

    return true;
}
