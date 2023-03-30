#include "HardMode.h"
#include "HardModeHandler.h"

#include "Modes/SelfCrafted.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"

#include <sstream>

bool HardModeMiscScript::CanSendAuctionHello(WorldSession const* session, ObjectGuid guid, Creature* creature)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
return true;
    }

    Player* player = session->GetPlayer();
    if (!player)
    {
        return true;
    }

    for (uint8 i = 0; i < DIFFICULTY_MODE_COUNT; ++i)
    {
        if (!sHardModeHandler->IsModeEnabled(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->CanSendAuctionHello(session, guid, creature);

        if (!result)
        {
            return false;
        }
    }

    return true;
}

ChatCommandTable HardModeCommandScript::GetCommands() const
{
    static ChatCommandTable sbCommandTable =
    {
        { "info", HandleHardModeInfoCommand, SEC_ADMINISTRATOR, Console::No },
        { "setmode", HandleHardModeSetModeCommand, SEC_ADMINISTRATOR, Console::No }
    };

    static ChatCommandTable commandTable =
    {
        { "hardmode", sbCommandTable }
    };

    return commandTable;
}

bool HardModeCommandScript::HandleHardModeInfoCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
{
    if (!target)
    {
        target = PlayerIdentifier::FromTargetOrSelf(handler);
    }

    if (!target)
    {
        return false;
    }

    if (!target->IsConnected())
    {
        return false;
    }

    auto targetPlayer = target->GetConnectedPlayer();

    std::stringstream ss;

    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sHardModeHandler->IsModeEnabled(targetPlayer, i))
        {
            ss << sHardModeHandler->GetNameFromMode(i);

            if (i != DifficultyModes::DIFFICULTY_MODE_COUNT - 1)
            {
                ss << ", ";
            }
        }
    }

    handler->SendSysMessage(Acore::StringFormatFmt("Enabled Difficulty Modes: {}", ss.str()));

    return true;
}

bool HardModeCommandScript::HandleHardModeSetModeCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 mode, uint8 value)
{
    if (!target)
    {
        target = PlayerIdentifier::FromTargetOrSelf(handler);
    }

    if (!target)
    {
        return false;
    }

    if (!target->IsConnected())
    {
        return false;
    }

    if (mode > (DifficultyModes::DIFFICULTY_MODE_COUNT - 1))
    {
        return false;
    }

    auto targetPlayer = target->GetConnectedPlayer();
    targetPlayer->UpdatePlayerSetting("HardMode", mode, value);

    return true;
}

void SC_AddHardModeScripts()
{
    sHardModeHandler->Modes[DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED] = new DifficultyModeSelfCrafted();

    new HardModeMiscScript();
    new HardModeCommandScript();
}
