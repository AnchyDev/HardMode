#include "HardMode.h"
#include "HardModeHandler.h"

#include "Modes/DifficultyModeSelfCrafted.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptedGossip.h"

#include <sstream>

void HardModePlayerScript::OnGiveXP(Player* player, uint32& amount, Unit* victim)
{
    sHardModeHandler->SetTainted(player, true);
}

void HardModePlayerScript::OnMoneyChanged(Player* player, int32& amount)
{
    sHardModeHandler->SetTainted(player, true);
}

bool HardModePlayerScript::CanSendMail(Player* player, ObjectGuid receiverGuid, ObjectGuid mailbox, std::string& subject, std::string& body, uint32 money, uint32 COD, Item* item)
{
    auto targetPlayer = ObjectAccessor::FindPlayer(receiverGuid);

    sHardModeHandler->SetTainted(player, true);

    if (targetPlayer)
    {
        sHardModeHandler->SetTainted(targetPlayer, true);
    }
    else
    {
        CharacterDatabase.Execute("INSERT INTO character_settings (guid, source, data) VALUES ({}, 'HardModeTainted', '1') ON DUPLICATE KEY UPDATE data = '1'", receiverGuid.GetRawValue());
    }

    return true;
}

bool HardModePlayerScript::CanEquipItem(Player* player, uint8 slot, uint16& dest, Item* pItem, bool swap, bool notLoading)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return true;
    }

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

        bool result = sHardModeHandler->Modes[i]->CanEquipItem(player, slot, dest, pItem, swap, notLoading);

        if (!result)
        {
            return false;
        }
    }

    return true;
}

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
        { "setmode", HandleHardModeSetModeCommand, SEC_ADMINISTRATOR, Console::No },
        { "settaint", HandleHardModeSetTaintCommand, SEC_ADMINISTRATOR, Console::No }
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
    handler->SendSysMessage(Acore::StringFormatFmt("IsTainted: {}", sHardModeHandler->IsTainted(targetPlayer)));

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

    handler->SendSysMessage(Acore::StringFormatFmt("Updated mode '{}' for player '{}' to '{}'.", sHardModeHandler->GetNameFromMode(mode), targetPlayer->GetName(), value));

    return true;
}

bool HardModeCommandScript::HandleHardModeSetTaintCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 value)
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
    sHardModeHandler->SetTainted(targetPlayer, value);

    handler->SendSysMessage(Acore::StringFormatFmt("Updated taint for player '{}' to '{}'.", targetPlayer->GetName(), value));

    return true;
}

bool HardModeGameObjectScript::OnGossipHello(Player* player, GameObject* go)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return false;
    }

    if (sConfigMgr->GetOption<bool>("HardMode.EnableSelfCrafted", false))
    {
        if (!sHardModeHandler->IsModeEnabled(player, DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED) && !sHardModeHandler->IsTainted(player))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Self-Crafted Mode", 0, DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED);
        }
        else if(sHardModeHandler->IsModeEnabled(player, DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Disable Self-Crafted Mode", 0, DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED);
        }
    }

    if (sHardModeHandler->IsTainted(player))
    {
        SendGossipMenuFor(player, HARDMODE_GOSSIP_NPCTEXT_ID_TAINTED, go->GetGUID());
    }
    else
    {
        SendGossipMenuFor(player, HARDMODE_GOSSIP_NPCTEXT_ID, go->GetGUID());
    }

    return true;
}

bool HardModeGameObjectScript::OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
{
    auto isModeEnabled = sHardModeHandler->IsModeEnabled(player, action);

    if (!isModeEnabled && sHardModeHandler->IsTainted(player))
    {
        ChatHandler(player->GetSession()).SendSysMessage("You cannot enable modes while tainted.");
        CloseGossipMenuFor(player);
    }
    else
    {
        player->UpdatePlayerSetting("HardMode", action, isModeEnabled == 0 ? 1 : 0);
        CloseGossipMenuFor(player);
    }

    return true;
}

void SC_AddHardModeScripts()
{
    sHardModeHandler->Modes[DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED] = new DifficultyModeSelfCrafted();

    new HardModeMiscScript();
    new HardModePlayerScript();
    new HardModeCommandScript();
    new HardModeGameObjectScript();
}
