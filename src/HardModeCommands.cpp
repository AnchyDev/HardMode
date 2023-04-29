#include "HardModeCommands.h"
#include "HardModeHandler.h"

#include "Chat.h"

ChatCommandTable HardModeCommandsScript::GetCommands() const
{
    static ChatCommandTable hmSetCommandTable =
    {
        { "mode", HandleHardModeSetModeCommand, SEC_MODERATOR, Console::No },
        { "taint", HandleHardModeSetTaintCommand, SEC_MODERATOR, Console::No },
        { "shadowban", HandleHardModeSetShadowBanCommand, SEC_MODERATOR, Console::No }
    };
    static ChatCommandTable hmCommandTable =
    {
        { "info", HandleHardModeInfoCommand, SEC_MODERATOR, Console::No },
        { "set", hmSetCommandTable }
    };

    static ChatCommandTable commandTable =
    {
        { "hardmode", hmCommandTable }
    };

    return commandTable;
}

bool HardModeCommandsScript::HandleHardModeInfoCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
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

    handler->SendSysMessage(Acore::StringFormatFmt("Enabled Difficulty Modes: {}", sHardModeHandler->GetNamesFromEnabledModes(targetPlayer)));
    handler->SendSysMessage(Acore::StringFormatFmt("IsTainted: {}", sHardModeHandler->IsPlayerTainted(targetPlayer->GetGUID())));
    handler->SendSysMessage(Acore::StringFormatFmt("IsShadowBanned: {}", sHardModeHandler->IsPlayerShadowBanned(targetPlayer->GetGUID())));

    return true;
}

bool HardModeCommandsScript::HandleHardModeSetModeCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 mode, uint8 value)
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

    if (!sHardModeHandler->GetHardModeFromId(mode))
    {
        return false;
    }

    auto targetPlayer = target->GetConnectedPlayer();
    sHardModeHandler->UpdateModeForPlayer(targetPlayer->GetGUID(), mode, value);

    handler->SendSysMessage(Acore::StringFormatFmt("Updated mode '{}' for player '{}' to '{}'.", sHardModeHandler->GetNameFromMode(mode), targetPlayer->GetName(), value));

    return true;
}

bool HardModeCommandsScript::HandleHardModeSetTaintCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 value)
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

    if (!sHardModeHandler->CanTaintPlayer(targetPlayer->GetGUID()))
    {
        return false;
    }

    sHardModeHandler->UpdatePlayerTainted(targetPlayer->GetGUID(), value);

    handler->SendSysMessage(Acore::StringFormatFmt("Updated taint for player '{}' to '{}'.", targetPlayer->GetName(), value));

    return true;
}

bool HardModeCommandsScript::HandleHardModeSetShadowBanCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 value)
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
    sHardModeHandler->UpdatePlayerShadowBanned(targetPlayer->GetGUID(), value);

    handler->SendSysMessage(Acore::StringFormatFmt("Updated shadow ban for player '{}' to '{}'.", targetPlayer->GetName(), value));

    return true;
}
