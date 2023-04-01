#include "DifficultyModeHardCore.h"
#include "HardMode.h"
#include "HardModeHandler.h"

#include "Config.h"
#include "Player.h"

DifficultyModeHardCore::DifficultyModeHardCore() : DifficultyMode(/*canBeTraded*/ false, /*canSendOrReceiveMail*/ false) { }

bool DifficultyModeHardCore::CanSendAuctionHello(WorldSession const* /*session*/, ObjectGuid /*guid*/, Creature* /*creature*/)
{
    return false;
}

bool DifficultyModeHardCore::CanGuildSendBankList(Guild const* /*guild*/, WorldSession* /*session*/, uint8 /*tabId*/, bool /*sendAllSlots*/)
{
    return false;
}

void DifficultyModeHardCore::OnPlayerResurrect(Player* /*player*/, float /*restorePercent*/, bool /*applySickness*/)
{
    return;
}

void DifficultyModeHardCore::OnPlayerReleasedGhost(Player* player)
{
    WorldLocation worldLoc(37, -614.38, -239.69, 379.35, 0.69); // Azshara Crater Shadow Realm
    player->TeleportTo(worldLoc);
    player->SetHomebind(worldLoc, 198 /* The Weeping Cave */);
    player->ResurrectPlayer(100, false);

    player->AddAura(HARDMODE_SPELL_AURA_SHADOWBAN, player); // Ghost effect, cannot be removed.

    sHardModeHandler->SetShadowBanned(player, true);
}

bool DifficultyModeHardCore::CanRepopAtGraveyard(Player* /*player*/)
{
    return false;
}

bool DifficultyModeHardCore::OnBeforeTeleport(Player* player, uint32 /*mapId*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (sHardModeHandler->IsShadowBanned(player))
    {
        return false;
    }

    return true;
}
