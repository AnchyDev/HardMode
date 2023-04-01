#include "HardMode.h"
#include "HardModeHandler.h"

#include "Modes/DifficultyModeSelfCrafted.h"
#include "Modes/DifficultyModeHardCore.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptedGossip.h"

#include <sstream>

void HardModePlayerScript::OnPVPKill(Player* killer, Player* victim)
{
    if (sHardModeHandler->HasModesEnabled(victim) && !sHardModeHandler->IsShadowBanned(killer))
    {
        if (sConfigMgr->GetOption<bool>("HardMode.AnnounceOnPvPKill", true))
        {
            sWorld->SendServerMessage(SERVER_MSG_STRING, Acore::StringFormatFmt("Player {} killed {} while they were doing the {} challenge(s)!", killer->GetName(), victim->GetName(), sHardModeHandler->GetNamesFromEnabledModes(victim)));
        }
    }
}

bool HardModePlayerScript::CanCastItemUseSpell(Player* player, Item* item, SpellCastTargets const& targets, uint8 castCount, uint32 glyphIndex)
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
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->CanCastItemUseSpell(player, item, targets, castCount, glyphIndex);

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot use that item while in {} mode.", sHardModeHandler->GetNameFromMode(i)));

            return false;
        }
    }

    return true;
}

void HardModePlayerScript::OnCreateItem(Player* player, Item* item, uint32 count)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    for (uint8 i = 0; i < DIFFICULTY_MODE_COUNT; ++i)
    {
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        sHardModeHandler->Modes[i]->OnCreateItem(player, item, count);
    }
}

bool HardModePlayerScript::CanGroupInvite(Player* player, std::string& memberName)
{
    Player* targetPlayer = ObjectAccessor::FindPlayerByName(memberName, true);

    if (!targetPlayer)
    {
        return false;
    }

    uint32 playerMask = sHardModeHandler->GetEnabledModesAsMask(player);
    uint32 targetPlayerMask = sHardModeHandler->GetEnabledModesAsMask(targetPlayer);

    if (playerMask != targetPlayerMask)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You cannot group with players that do not have the same hard modes enabled as you.");

        return false;
    }

    uint32 levelRange = sConfigMgr->GetOption<uint32>("HardMode.PartyLevelRange", 3);
    if (levelRange)
    {
        uint32 currentRange = std::abs(int32(player->GetLevel() - targetPlayer->GetLevel()));

        if (currentRange > levelRange)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot group with players who are further than {} levels from you.", levelRange));
            return false;
        }
    }

    sHardModeHandler->SetTainted(player, true);
    sHardModeHandler->SetTainted(targetPlayer, true);

    return true;
}

bool HardModePlayerScript::OnBeforeTeleport(Player* player, uint32 mapId, float x, float y, float z, float orientation, uint32 options, Unit* target)
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
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->OnBeforeTeleport(player, mapId, x, y, z, orientation, options, target);

        if (!result)
        {
            return false;
        }
    }

    return true;
}

bool HardModePlayerScript::CanRepopAtGraveyard(Player* player)
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
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->CanRepopAtGraveyard(player);

        if (!result)
        {
            return false;
        }
    }

    return true;
}

void HardModePlayerScript::OnPlayerReleasedGhost(Player* player)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    for (uint8 i = 0; i < DIFFICULTY_MODE_COUNT; ++i)
    {
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        sHardModeHandler->Modes[i]->OnPlayerReleasedGhost(player);
    }
}

void HardModePlayerScript::OnPlayerResurrect(Player* player, float restorePercent, bool applySickness)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    for (uint8 i = 0; i < DIFFICULTY_MODE_COUNT; ++i)
    {
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        sHardModeHandler->Modes[i]->OnPlayerResurrect(player, restorePercent, applySickness);
    }
}

void HardModePlayerScript::OnLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    uint8 level = player->GetLevel();
    uint8 maxLevel = sConfigMgr->GetOption<int32>("MaxPlayerLevel", DEFAULT_MAX_LEVEL);

    if (level < maxLevel)
    {
        return;
    }

    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sHardModeHandler->IsModeEnabledForPlayer(player, i))
        {
            sHardModeHandler->RewardPlayerForMode(player, i);
        }
    }

    if (sHardModeHandler->HasModesEnabled(player))
    {
        ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You have reached max level in the modes: {}.", sHardModeHandler->GetNamesFromEnabledModes(player)));
        ChatHandler(player->GetSession()).SendSysMessage("Your rewards have been mailed to you, return to the Shrine of Hard Mode to disable your modes.");

        if (sConfigMgr->GetOption<bool>("HardMode.AnnounceModesMaxLevel", true))
        {
            sWorld->SendServerMessage(SERVER_MSG_STRING, Acore::StringFormatFmt("Congratulations, player {} reached level {} while in: {}!", player->GetName(), maxLevel, sHardModeHandler->GetNamesFromEnabledModes(player)));
        }
    }
}

void HardModePlayerScript::OnLogin(Player* player)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!sConfigMgr->GetOption<bool>("HardMode.AnnounceModesOnLogin", true))
    {
        return;
    }

    if (sHardModeHandler->HasModesEnabled(player))
    {
        ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("Enabled Difficulty Modes: {}", sHardModeHandler->GetNamesFromEnabledModes(player)));
    }
}

void HardModePlayerScript::OnGiveXP(Player* player, uint32& /*amount*/, Unit* /*victim*/)
{
    if (player->getClass() == CLASS_DEATH_KNIGHT && player->GetMapId() == HARDMODE_ZONE_EBONHOLD)
    {
        if (!player->IsQuestRewarded(HARDMODE_DEATHKNIGHT_INITIAL_QUEST))
        {
            return;
        }
    }

    sHardModeHandler->SetTainted(player, true);
}

void HardModePlayerScript::OnMoneyChanged(Player* player, int32& /*amount*/)
{
    sHardModeHandler->SetTainted(player, true);
}

bool HardModePlayerScript::CanInitTrade(Player* player, Player* target)
{
    for (uint8 i = 0; i < DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sHardModeHandler->IsModeEnabledForPlayer(player, i))
        {
            bool isTradable = sHardModeHandler->Modes[i]->IsTradable();

            if (!isTradable)
            {
                ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot trade players while in the {} mode.", sHardModeHandler->GetNameFromMode(i)));

                return false;
            }
        }

        if (sHardModeHandler->IsModeEnabledForPlayer(target, i))
        {
            bool isTradable = sHardModeHandler->Modes[i]->IsTradable();

            if (!isTradable)
            {
                ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot trade players that are in the {} mode.", sHardModeHandler->GetNameFromMode(i)));

                return false;
            }
        }
    }

    sHardModeHandler->SetTainted(player, true);
    sHardModeHandler->SetTainted(target, true);

    return true;
}

bool HardModePlayerScript::CanSendMail(Player* player, ObjectGuid receiverGuid, ObjectGuid /*mailbox*/, std::string& /*subject*/, std::string& /*body*/, uint32 /*money*/, uint32 /*COD*/, Item* /*item*/)
{
    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        auto isMailable = sHardModeHandler->Modes[i]->IsMailable();

        if (sHardModeHandler->IsModeEnabledForPlayer(player, i) && !isMailable)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot send mail while in the {} mode.", sHardModeHandler->GetNameFromMode(i)));

            return false;
        }
    }

    auto targetPlayer = ObjectAccessor::FindPlayer(receiverGuid);

    bool canMail = true;
    uint8 receiverMode = 0;

    if (targetPlayer)
    {
        for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
        {
            auto isMailable = sHardModeHandler->Modes[i]->IsMailable();

            if (sHardModeHandler->IsModeEnabledForPlayer(targetPlayer, i) && !isMailable)
            {
                receiverMode = i;
                canMail = false;
                break;
            }
        }
    }
    else
    {
        auto playerSettings = sHardModeHandler->GetPlayerSettingsFromDatabase(receiverGuid);

        if (!playerSettings)
        {
            return true;
        }

        auto modes = playerSettings->find("HardMode")->second;
        if (modes.size() < DifficultyModes::DIFFICULTY_MODE_COUNT)
        {
            return true;
        }

        for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
        {
            auto isMailable = sHardModeHandler->Modes[i]->IsMailable();

            if (modes[i].value > 0 && !isMailable)
            {
                receiverMode = i;
                canMail = false;
                break;
            }
        }
    }

    if (!canMail)
    {
        ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot send mail to a {} mode player.", sHardModeHandler->GetNameFromMode(receiverMode)));

        return false;
    }

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
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->CanEquipItem(player, slot, dest, pItem, swap, notLoading);

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot equip that item while in {} mode.", sHardModeHandler->GetNameFromMode(i)));

            return false;
        }
    }

    return true;
}

bool HardModeGuildScript::CanGuildSendBankList(Guild const* guild, WorldSession* session, uint8 tabId, bool sendAllSlots)
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
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->CanGuildSendBankList(guild, session, tabId, sendAllSlots);

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot use the guild bank while in {} mode.", sHardModeHandler->GetNameFromMode(i)));

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
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->CanSendAuctionHello(session, guid, creature);

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("You cannot use the auction house while in {} mode.", sHardModeHandler->GetNameFromMode(i)));

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
        { "settaint", HandleHardModeSetTaintCommand, SEC_ADMINISTRATOR, Console::No },
        { "setshadowban", HandleHardModeSetShadowBanCommand, SEC_ADMINISTRATOR, Console::No }
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

    handler->SendSysMessage(Acore::StringFormatFmt("Enabled Difficulty Modes: {}", sHardModeHandler->GetNamesFromEnabledModes(targetPlayer)));
    handler->SendSysMessage(Acore::StringFormatFmt("IsTainted: {}", sHardModeHandler->IsTainted(targetPlayer)));
    handler->SendSysMessage(Acore::StringFormatFmt("IsShadowBanned: {}", sHardModeHandler->IsShadowBanned(targetPlayer)));

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

    if (mode == DifficultyModes::DIFFICULTY_MODE_HARDCORE)
    {
        if (value)
        {
            targetPlayer->AddAura(HARDMODE_SPELL_AURA_HARDCORE, targetPlayer);
        }
        else
        {
            targetPlayer->RemoveAura(HARDMODE_SPELL_AURA_HARDCORE);
        }
    }

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

bool HardModeCommandScript::HandleHardModeSetShadowBanCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, uint8 value)
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
    sHardModeHandler->SetShadowBanned(targetPlayer, value);

    handler->SendSysMessage(Acore::StringFormatFmt("Updated shadow ban for player '{}' to '{}'.", targetPlayer->GetName(), value));

    return true;
}

bool HardModeGameObjectScript::OnGossipHello(Player* player, GameObject* go)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return false;
    }

    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (sConfigMgr->GetOption<bool>(sHardModeHandler->GetConfigNameFromMode(i), false))
        {
            if (!sHardModeHandler->IsModeEnabledForPlayer(player, i) && !sHardModeHandler->IsTainted(player))
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, Acore::StringFormatFmt("Enable {} Mode", sHardModeHandler->GetNameFromMode(i)), 0, i);
            }
            else if (sHardModeHandler->IsModeEnabledForPlayer(player, i))
            {
                if (sHardModeHandler->IsTainted(player))
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, Acore::StringFormatFmt("Disable {} Mode", sHardModeHandler->GetNameFromMode(i)), 0, i, Acore::StringFormatFmt("Are you sure you want to disable the {} mode?|n|nYou won't be able to re-enable this mode.", sHardModeHandler->GetNameFromMode(i)), 0, false);
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, Acore::StringFormatFmt("Disable {} Mode", sHardModeHandler->GetNameFromMode(i)), 0, i);
                }
            }
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

bool HardModeGameObjectScript::OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 action)
{
    auto isModeEnabled = sHardModeHandler->IsModeEnabledForPlayer(player, action);

    if (!isModeEnabled && sHardModeHandler->IsTainted(player))
    {
        ChatHandler(player->GetSession()).SendSysMessage("You cannot enable modes while tainted.");
        CloseGossipMenuFor(player);
    }
    else
    {
        player->UpdatePlayerSetting("HardMode", action, isModeEnabled == 0 ? 1 : 0);
        CloseGossipMenuFor(player);

        if (action == DifficultyModes::DIFFICULTY_MODE_HARDCORE)
        {
            if (!isModeEnabled)
            {
                player->AddAura(HARDMODE_SPELL_AURA_HARDCORE, player);
            }
            else
            {
                player->RemoveAura(HARDMODE_SPELL_AURA_HARDCORE);
            }
        }
    }

    return true;
}

void HardModeWorldScript::OnAfterConfigLoad(bool reload)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    sHardModeHandler->LoadRewardsFromDatabase();

    for (uint8 i = 0; i < DIFFICULTY_MODE_COUNT; ++i)
    {
        if (!sConfigMgr->GetOption<bool>(sHardModeHandler->GetConfigNameFromMode(i), false))
        {
            continue;
        }

        sHardModeHandler->Modes[i]->OnAfterConfigLoad(reload);
    }
}

void SC_AddHardModeScripts()
{
    sHardModeHandler->Modes[DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED] = new DifficultyModeSelfCrafted();
    sHardModeHandler->Modes[DifficultyModes::DIFFICULTY_MODE_HARDCORE] = new DifficultyModeHardCore();

    new HardModeMiscScript();
    new HardModeGuildScript();
    new HardModeWorldScript();
    new HardModePlayerScript();
    new HardModeCommandScript();
    new HardModeGameObjectScript();
}
