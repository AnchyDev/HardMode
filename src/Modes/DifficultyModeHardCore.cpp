#include "DifficultyModeHardCore.h"
#include "HardMode.h"
#include "HardModeHandler.h"

#include "Config.h"
#include "Player.h"

DifficultyModeHardCore::DifficultyModeHardCore() : DifficultyMode(/*canBeTraded*/ false, /*canSendOrReceiveMail*/ false, /*canCrossPlay*/ false) { }

bool DifficultyModeHardCore::CanGroupInvite(Player* player, Player* targetPlayer)
{
    bool flag1 = sHardModeHandler->IsModeEnabledForPlayer(player, DifficultyModes::DIFFICULTY_MODE_HARDCORE);
    bool flag2 = sHardModeHandler->IsModeEnabledForPlayer(targetPlayer, DifficultyModes::DIFFICULTY_MODE_HARDCORE);

    return flag1 == flag2;
}

bool DifficultyModeHardCore::CanSendAuctionHello(WorldSession const* /*session*/, ObjectGuid /*guid*/, Creature* /*creature*/)
{
    return false;
}

bool DifficultyModeHardCore::CanGuildSendBankList(Guild const* /*guild*/, WorldSession* /*session*/, uint8 /*tabId*/, bool /*sendAllSlots*/)
{
    return false;
}

void DifficultyModeHardCore::OnPlayerResurrect(Player* player, float /*restorePercent*/, bool /*applySickness*/)
{
    OnPlayerReleasedGhost(player); // Shadowban the player.
}

void DifficultyModeHardCore::OnPlayerReleasedGhost(Player* player)
{
    WorldLocation worldLoc(37, -614.38, -239.69, 379.35, 0.69); // Azshara Crater Shadow Realm
    player->TeleportTo(worldLoc);
    player->SetHomebind(worldLoc, 198 /* The Weeping Cave */);

    sHardModeHandler->SetShadowBanned(player, true); // Shadowban after teleport, otherwise it won't teleport.

    if (!player->IsAlive())
    {
        player->ResurrectPlayer(100, false);
        player->RemoveCorpse();
    }

    player->AddAura(HARDMODE_SPELL_AURA_SHADOWBAN, player); // Ghost effect, cannot be removed.
}

bool DifficultyModeHardCore::CanRepopAtGraveyard(Player* /*player*/)
{
    return false;
}

bool DifficultyModeHardCore::OnBeforeTeleport(Player* player, uint32 mapId, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (sHardModeHandler->IsShadowBanned(player))
    {
        return (mapId == 37); // Only allow teleports for Shadowban players if it's to the shadow realm.
    }

    return true;
}

void DifficultyModeHardCore::AddEffectsForPlayer(Player* player)
{
    if (!player)
    {
        return;
    }

    if (!sHardModeHandler->IsShadowBanned(player))
    {
        if (!player->HasAura(HARDMODE_SPELL_AURA_HARDCORE))
        {
            player->AddAura(HARDMODE_SPELL_AURA_HARDCORE, player);
        }
    }
    else
    {
        if (!player->HasAura(HARDMODE_SPELL_AURA_SHADOWBAN))
        {
            player->AddAura(HARDMODE_SPELL_AURA_SHADOWBAN, player);
        }
    }
}

void DifficultyModeHardCore::RemoveEffectsForPlayer(Player* player)
{
    if (!player)
    {
        return;
    }

    if (player->HasAura(HARDMODE_SPELL_AURA_HARDCORE))
    {
        player->RemoveAura(HARDMODE_SPELL_AURA_HARDCORE);
    }

    if (player->HasAura(HARDMODE_SPELL_AURA_SHADOWBAN))
    {
        player->RemoveAura(HARDMODE_SPELL_AURA_SHADOWBAN);
    }
}

void DifficultyModeHardCore::OnAfterConfigLoad(bool reload)
{
    _canBeTraded = sConfigMgr->GetOption<bool>("HardMode.HardCore.CanBeTraded", false);
    _canSendOrReceiveMail = sConfigMgr->GetOption<bool>("HardMode.HardCore.CanSendRecMail", false);
    _canCrossPlay = sConfigMgr->GetOption<bool>("HardMode.HardCore.CanCrossPlay", false);
}
