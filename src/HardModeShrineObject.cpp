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

        bool flag = sHardModeHandler->IsModeEnabledForPlayer(player->GetGUID(), mode.Id);
        std::string state = flag ? "Disable" : "Enable";
        std::string format = Acore::StringFormatFmt("{} {} mode.", state, mode.Name);

        if (sHardModeHandler->IsPlayerTainted(player->GetGUID()))
        {
            if (flag)
            {
                std::string popupFormat = Acore::StringFormatFmt("Are you sure you want to disable the {} mode?|n|nYou will not be able to re-enable it.", mode.Name);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, format, 0, mode.Id, popupFormat, 0, false);
            }
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, format, 0, mode.Id);
        }
    }

    if (!sHardModeHandler->IsPlayerTainted(player->GetGUID()))
    {
        SendGossipMenuFor(player, HARDMODE_SHRINE_GREETING, go->GetGUID());
    }
    else
    {
        SendGossipMenuFor(player, HARDMODE_SHRINE_GREETING_TAINTED, go->GetGUID());
    }

    return true;
}

bool HardModeShrineObject::OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 mode)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return false;
    }

    bool flag = sHardModeHandler->IsModeEnabledForPlayer(player->GetGUID(), mode);

    if (!flag && sHardModeHandler->IsPlayerTainted(player->GetGUID()))
    {
        sHardModeHandler->SendAlert(player, "You cannot enable hard modes while tainted.");
    }
    else
    {
        sHardModeHandler->UpdateModeForPlayer(player->GetGUID(), mode, !flag);
    }

    CloseGossipMenuFor(player);

    return true;
}
