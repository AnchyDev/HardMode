#include "HardMode.h"
#include "HardModeHandler.h"

#include "Modes/DifficultyModeSelfCrafted.h"
#include "Modes/DifficultyModeHardCore.h"
#include "Modes/DifficultyModeSlowXP.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "Spell.h"
#include "ScriptedGossip.h"

#include <sstream>

void HardModePlayerScript::OnPVPKill(Player* killer, Player* victim)
{
    if (sHardModeHandler->HasModesEnabled(victim) && !sHardModeHandler->IsShadowBanned(killer))
    {
        if (sConfigMgr->GetOption<bool>("HardMode.Announce.OnPvPKill", true))
        {
            std::string formatMsg = Acore::StringFormatFmt("|cffFFFFFFPlayer |cff00FF00{}|cffFFFFFF killed |cff00FF00{}|cffFFFFFF while they were doing the {}|cffFFFFFF challenge(s)!", killer->GetName(), victim->GetName(), sHardModeHandler->GetNamesFromEnabledModes(victim, true));

            sWorld->SendServerMessage(SERVER_MSG_STRING, formatMsg);

            //if (sConfigMgr->GetOption<bool>("HardMode.Webhook.Enable", false))
            //{
            //    std::string whMsg = sHardModeHandler->CreateWebhookObject("pvp kill", "some death occurred.");
            //    sHardModeHandler->SendWebhookMessage(whMsg);
            //}
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
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot use that item while in {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

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

    for (uint8 i = 0; i < DifficultyModes::DIFFICULTY_MODE_COUNT; ++i)
    {
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        bool result = sHardModeHandler->Modes[i]->CanGroupInvite(player, targetPlayer);

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage("|cffFFFFFFYou cannot group with players that do not have the same hard modes enabled as you.");

            return false;
        }
    }

    uint32 levelRange = sConfigMgr->GetOption<uint32>("HardMode.PartyLevelRange", 3);
    if (levelRange)
    {
        uint32 currentRange = std::abs(int32(player->GetLevel() - targetPlayer->GetLevel()));

        if (currentRange > levelRange)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot group with players who are further than |r{}|cffFFFFFF levels from you.", levelRange));
            return false;
        }
    }

    sHardModeHandler->SetTainted(player, true);
    sHardModeHandler->SetTainted(targetPlayer, true);

    return true;
}

bool HardModePlayerScript::CanJoinLfg(Player* player, uint8 roles, lfg::LfgDungeonSet& dungeons, const std::string& comment)
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

        bool result = sHardModeHandler->Modes[i]->CanJoinLfg(player, roles, dungeons, comment);

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot queue Random Dungeon Finder while in {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

            return false;
        }
    }

    return true;
}

void HardModeUnitScript::OnDamage(Unit* attacker, Unit* victim, uint32& damage)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!attacker || !victim)
    {
        return;
    }

    if (!attacker->IsPlayer() || !victim->IsPlayer())
    {
        return;
    }

    Player* pAttacker = (Player*)attacker;
    Player* pVictim = (Player*)victim;

    if (!sHardModeHandler->TestForCrossplay(pVictim, pAttacker))
    {
        damage = 0;
    }
}

bool HardModeSpellScript::CanPrepare(Spell* spell, SpellCastTargets const* targets, AuraEffect const* /*triggeredByAura*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return true;
    }

    if (!spell)
    {
        return true;
    }

    if (!targets)
    {
        return true;
    }

    Unit* target = targets->GetUnitTarget();
    Unit* caster = spell->GetCaster();

    if (!target || !caster)
    {
        return true;
    }

    if (!target->IsPlayer() || !caster->IsPlayer())
    {
        return true;
    }

    Player* pTarget = (Player*)target;
    Player* pCaster = (Player*)caster;

    if (!sHardModeHandler->TestForCrossplay(pTarget, pCaster))
    {
        sHardModeHandler->SendAlert(pCaster, "You cannot cast spells at players in modes other than your own.");
        return false;
    }

    return true;
}

void HardModeUnitScript::OnHeal(Unit* healer, Unit* receiver, uint32& gain)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!healer || !receiver)
    {
        return;
    }

    if (!healer->IsPlayer() || !receiver->IsPlayer())
    {
        return;
    }

    Player* pHealer = (Player*)healer;
    Player* pReceiver = (Player*)receiver;

    if (!sHardModeHandler->TestForCrossplay(pReceiver, pHealer))
    {
        sHardModeHandler->SendAlert(pHealer, "You cannot heal players in modes other than your own.");
        gain = 0;
    }
}

void HardModeUnitScript::ModifyHealReceived(Unit* receiver, Unit* healer, uint32& gain, SpellInfo const* /*spellInfo*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!healer || !receiver)
    {
        return;
    }

    if (!healer->IsPlayer() || !receiver->IsPlayer())
    {
        return;
    }

    Player* pHealer = (Player*)healer;
    Player* pReceiver = (Player*)receiver;

    if (!sHardModeHandler->TestForCrossplay(pReceiver, pHealer))
    {
        sHardModeHandler->SendAlert(pHealer, "You cannot heal players in modes other than your own.");
        gain = 0;
    }
}

void HardModeUnitScript::ModifyPeriodicDamageAurasTick(Unit* victim, Unit* attacker, uint32& damage, SpellInfo const* /*spellInfo*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!attacker || !victim)
    {
        return;
    }

    if (!attacker->IsPlayer() || !victim->IsPlayer())
    {
        return;
    }

    Player* pAttacker = (Player*)attacker;
    Player* pVictim = (Player*)victim;

    if (!sHardModeHandler->TestForCrossplay(pVictim, pAttacker))
    {
        sHardModeHandler->SendAlert(pAttacker, "You cannot attack players in modes other than your own.");
        damage = 0;
    }
}

void HardModeUnitScript::ModifyMeleeDamage(Unit* victim, Unit* attacker, uint32& damage)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!victim || !attacker)
    {
        return;
    }

    if (!attacker->IsPlayer() || !victim->IsPlayer())
    {
        return;
    }

    Player* pAttacker = (Player*)attacker;
    Player* pVictim = (Player*)victim;

    if (!sHardModeHandler->TestForCrossplay(pVictim, pAttacker))
    {
        sHardModeHandler->SendAlert(pAttacker, "You cannot attack players in modes other than your own.");
        damage = 0;
    }
}

void HardModeUnitScript::ModifySpellDamageTaken(Unit* victim, Unit* attacker, int32& damage, SpellInfo const* /*spellInfo*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (!attacker || !victim)
    {
        return;
    }

    if (!attacker->IsPlayer() || !victim->IsPlayer())
    {
        return;
    }

    Player* pAttacker = (Player*)attacker;
    Player* pVictim = (Player*)victim;

    if (!sHardModeHandler->TestForCrossplay(pVictim, pAttacker))
    {
        sHardModeHandler->SendAlert(pAttacker, "You cannot attack players in modes other than your own.");
        damage = 0;
    }
}

uint32 HardModeUnitScript::DealDamage(Unit* attacker, Unit* victim, uint32 damage, DamageEffectType /*damagetype*/)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return damage;
    }

    if (!attacker || !victim)
    {
        return damage;
    }

    if (!attacker->IsPlayer() || !victim->IsPlayer())
    {
        return damage;
    }

    Player* pAttacker = (Player*)attacker;
    Player* pVictim = (Player*)victim;

    if (!sHardModeHandler->TestForCrossplay(pVictim, pAttacker))
    {
        sHardModeHandler->SendAlert(pAttacker, "You cannot attack players in modes other than your own.");
        damage = 0;
    }

    return damage;
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

        if (sConfigMgr->GetOption<bool>("HardMode.Announce.MaxLevel", true))
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

    if (!sConfigMgr->GetOption<bool>("HardMode.Announce.ModesOnLogin", true))
    {
        return;
    }

    if (sHardModeHandler->HasModesEnabled(player))
    {
        ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFEnabled Difficulty Modes: {}", sHardModeHandler->GetNamesFromEnabledModes(player, true)));

        sHardModeHandler->UpdateAllModeEffects(player);
    }
}

void HardModePlayerScript::OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 xpSource)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return;
    }

    if (player->getClass() == CLASS_DEATH_KNIGHT && player->GetMapId() == HARDMODE_ZONE_EBONHOLD)
    {
        if (!player->IsQuestRewarded(HARDMODE_DEATHKNIGHT_INITIAL_QUEST))
        {
            return;
        }
    }

    for (uint8 i = 0; i < DIFFICULTY_MODE_COUNT; ++i)
    {
        if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(player, i))
        {
            continue;
        }

        sHardModeHandler->Modes[i]->OnGiveXP(player, amount, victim, xpSource);
    }

    sHardModeHandler->SetTainted(player, true);
}

void HardModePlayerScript::OnQuestComputeXP(Player* player, Quest const* quest, uint32& xpValue)
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

        sHardModeHandler->Modes[i]->OnQuestComputeXP(player, quest, xpValue);
    }
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
                ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot trade players while in the {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

                return false;
            }
        }

        if (sHardModeHandler->IsModeEnabledForPlayer(target, i))
        {
            bool isTradable = sHardModeHandler->Modes[i]->IsTradable();

            if (!isTradable)
            {
                ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot trade players that are in the {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

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
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot send mail while in the {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

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

        auto it = playerSettings->find("HardMode");
        if (it == playerSettings->end())
        {
            return true;
        }

        auto modes = it->second;

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
        ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot send mail to a {}{} |cffFFFFFFmode player.", sHardModeHandler->GetColorFromMode(receiverMode), sHardModeHandler->GetNameFromMode(receiverMode)));

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
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot equip that item while in {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

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

    if (!session)
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
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot use the guild bank while in {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

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
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFYou cannot use the auction house while in {}{} |cffFFFFFFmode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)));

            return false;
        }
    }

    return true;
}

ChatCommandTable HardModeCommandScript::GetCommands() const
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

    handler->SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFEnabled Difficulty Modes: {}", sHardModeHandler->GetNamesFromEnabledModes(targetPlayer, true)));
    handler->SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFIsTainted: |r{}", sHardModeHandler->IsTainted(targetPlayer)));
    handler->SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFIsShadowBanned: |r{}", sHardModeHandler->IsShadowBanned(targetPlayer)));

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

    sHardModeHandler->UpdateAllModeEffects(targetPlayer);

    handler->SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFUpdated mode '{}{}|cffFFFFFF' for player '|cff00FF00{}|cffFFFFFF' to '|r{}|cffFFFFFF'.", sHardModeHandler->GetColorFromMode(mode), sHardModeHandler->GetNameFromMode(mode), targetPlayer->GetName(), value));

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

    handler->SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFUpdated taint for player '|cff00FF00{}|cffFFFFFF' to '|r{}|cffFFFFFF'.", targetPlayer->GetName(), value));

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

    handler->SendSysMessage(Acore::StringFormatFmt("|cffFFFFFFUpdated shadow ban for player '|cff00FF00{}|cffFFFFFF' to '|r{}|cffFFFFFF'.", targetPlayer->GetName(), value));

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
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, Acore::StringFormatFmt("Enable {}{}|r Mode", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)), 0, i);
            }
            else if (sHardModeHandler->IsModeEnabledForPlayer(player, i))
            {
                if (sHardModeHandler->IsTainted(player))
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, Acore::StringFormatFmt("Disable {}{}|r Mode", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)), 0, i, Acore::StringFormatFmt("Are you sure you want to disable the {}{}|r mode?|n|nYou won't be able to re-enable this mode.", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)), 0, false);
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, Acore::StringFormatFmt("Disable {}{}|r Mode", sHardModeHandler->GetColorFromMode(i), sHardModeHandler->GetNameFromMode(i)), 0, i);
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

        sHardModeHandler->UpdateAllModeEffects(player);
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

bool HardModeServerScript::CanPacketSend(WorldSession* session, WorldPacket& packet)
{
    if (!sConfigMgr->GetOption<bool>("HardMode.Enable", false))
    {
        return true;
    }

    if (!sConfigMgr->GetOption<bool>("HardMode.OverrideWhoList", true))
    {
        return true;
    }

    if (!session)
    {
        return true;
    }

    auto player = session->GetPlayer();
    if (!player)
    {
        return true;
    }

    auto opCode = packet.GetOpcode();
    if (opCode != SMSG_WHO)
    {
        return true;
    }

    uint32 ignorePacketFlag = 696969;

    bool ignorePacket = packet.read<uint32>(packet.size() - 4) == ignorePacketFlag;

    // Read position is moved as this packet is modified already, don't read any further.
    if (ignorePacket)
    {
        return true;
    }

    uint32 displayCount = packet.read<uint32>();
    uint32 matchCount = packet.read<uint32>();

    // No matches, don't try read any further.
    if (matchCount == 0 && displayCount == 0)
    {
        return false;
    }
    uint32 unknownZoneId = 4988;
    uint32 azsharaCraterZoneId = 268;
    uint32 jailZoneId = 3888;

    for (uint32 i = 0; i < displayCount; ++i)
    {
        std::string playerName = packet.read<std::string>();
        packet.read_skip<std::string>(); //GuildName
        packet.read_skip<uint32>(); //PlayerLvl
        packet.read_skip<uint32>(); //PlayerClass
        packet.read_skip<uint32>(); //PlayerRace
        packet.read_skip<uint8>(); //PlayerGender
        uint32 playerZoneId = packet.read<uint32>();

        if (playerZoneId == azsharaCraterZoneId)
        {
            packet.put(packet.rpos() - 4, jailZoneId);
        }
        else
        {
            auto targetPlayer = ObjectAccessor::FindPlayerByName(playerName);

            if (!targetPlayer)
            {
                return true;
            }

            if (!sHardModeHandler->IsModeEnabledForPlayerAndServer(targetPlayer, DifficultyModes::DIFFICULTY_MODE_HARDCORE))
            {
                return true;
            }

            packet.put(packet.rpos() - 4, unknownZoneId);
        }
    }

    // Add ignore packet flag to end of payload.
    packet << ignorePacketFlag;

    // Resend modified packet.
    player->SendDirectMessage(&packet);

    return false;
}

void SC_AddHardModeScripts()
{
    sHardModeHandler->Modes[DifficultyModes::DIFFICULTY_MODE_SELF_CRAFTED] = new DifficultyModeSelfCrafted();
    sHardModeHandler->Modes[DifficultyModes::DIFFICULTY_MODE_HARDCORE] = new DifficultyModeHardCore();
    sHardModeHandler->Modes[DifficultyModes::DIFFICULTY_MODE_SLOWXP] = new DifficultyModeSlowXP();

    new HardModeUnitScript();
    new HardModeMiscScript();
    new HardModeSpellScript();
    new HardModeGuildScript();
    new HardModeWorldScript();
    new HardModeServerScript();
    new HardModePlayerScript();
    new HardModeCommandScript();
    new HardModeGameObjectScript();
}
