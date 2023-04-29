#include "HardModeHooks/HardModeHooksUnit.h"
#include "HardModeHandler.h"

void HardModeHooksUnitScript::OnAuraRemove(Unit* unit, AuraApplication* /*auraApp*/, AuraRemoveMode mode)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!unit)
    {
        return;
    }

    if (!unit->IsPlayer())
    {
        return;
    }

    // Don't reapply aura on death, resurrection handles reapplication already.
    if (mode == AURA_REMOVE_BY_DEATH)
    {
        return;
    }

    Player* player = unit->ToPlayer();
    if (!player || !player->IsInWorld())
    {
        return;
    }

    sHardModeHandler->ValidatePlayerAuras(player);
}

void HardModeHooksUnitScript::OnDamage(Unit* attacker, Unit* /*victim*/, uint32& /*damage*/)
{
    if (!sHardModeHandler->IsHardModeEnabled())
    {
        return;
    }

    if (!attacker)
    {
        return;
    }

    if (!attacker->IsPlayer())
    {
        return;
    }

    Player* player = attacker->ToPlayer();
    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_PACIFIST))
    {
        auto modes = sHardModeHandler->GetPlayerModesFromRestriction(player->GetGUID(), HARDMODE_RESTRICT_PACIFIST);
        for (auto mode : modes)
        {
            if (!mode.Enabled)
            {
                continue;
            }

            sHardModeHandler->UpdateModeForPlayer(player->GetGUID(), mode.Id, false);
        }

        // TODO: Update this to alert in the chat so the player is more aware.
        sHardModeHandler->SendAlert(player, "You have failed the pacifist challenge.");
        player->AddAura(HARDMODE_AURA_PACIFIST_FAIL, player);
    }
}
