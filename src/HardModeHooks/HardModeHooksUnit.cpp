#include "HardModeHooks/HardModeHooksUnit.h"
#include "HardModeHandler.h"

void HardModeHooksUnitScript::OnAuraApply(Unit* unit, Aura* /*aura*/)
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

    Player* player = unit->ToPlayer();
    if (!player || !player->IsInWorld())
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SMALLFISH))
    {
        // Schedule due to issues..
        sHardModeHandler->GetScheduler()->Schedule(50ms, [player](TaskContext /*task*/)
        {
            sHardModeHandler->UpdatePlayerScaleSpeed(player, SMALLFISH_SCALE);
        });
    }
}

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

    Player* player = unit->ToPlayer();

    if (!player || !player->IsInWorld())
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SMALLFISH))
    {
        // Schedule due to issues..
        sHardModeHandler->GetScheduler()->Schedule(50ms, [player](TaskContext /*task*/)
        {
            sHardModeHandler->UpdatePlayerScaleSpeed(player, SMALLFISH_SCALE);
        });
    }

    // Don't reapply aura on death, resurrection handles reapplication already.
    if (mode == AURA_REMOVE_BY_DEATH)
    {
        return;
    }

    sHardModeHandler->ValidatePlayerAuras(player);
}

void HardModeHooksUnitScript::OnDamage(Unit* attacker, Unit* target, uint32& damage)
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

    auto targetPlayer = target->ToPlayer();

    if (!targetPlayer)
    {
        return;
    }

    if (!sHardModeHandler->HasMatchingModesWithRestriction(player, targetPlayer, HARDMODE_RESTRICT_BLOCK_CROSSPVP))
    {
        damage = 0;
        sHardModeHandler->SendAlert(player, "You cannot damage players in other modes than your own.");
    }
}

void HardModeHooksUnitScript::ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* /*spellInfo*/)
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

    auto player = attacker->ToPlayer();

    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SMALLFISH))
    {
        damage = damage * SMALLFISH_SCALE;
    }

    auto targetPlayer = target->ToPlayer();

    if (!targetPlayer)
    {
        return;
    }

    if (!sHardModeHandler->HasMatchingModesWithRestriction(player, targetPlayer, HARDMODE_RESTRICT_BLOCK_CROSSPVP))
    {
        damage = 0;
        sHardModeHandler->SendAlert(player, "You cannot damage players in other modes than your own.");
    }
}

void HardModeHooksUnitScript::ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage)
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

    auto player = attacker->ToPlayer();

    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SMALLFISH))
    {
        damage = damage * SMALLFISH_SCALE;
    }

    auto targetPlayer = target->ToPlayer();

    if (!targetPlayer)
    {
        return;
    }

    if (!sHardModeHandler->HasMatchingModesWithRestriction(player, targetPlayer, HARDMODE_RESTRICT_BLOCK_CROSSPVP))
    {
        damage = 0;
        sHardModeHandler->SendAlert(player, "You cannot damage players in other modes than your own.");
    }
}

void HardModeHooksUnitScript::ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* /*spellInfo*/)
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

    auto player = attacker->ToPlayer();

    if (!player)
    {
        return;
    }

    if (sHardModeHandler->PlayerHasRestriction(player->GetGUID(), HARDMODE_RESTRICT_SMALLFISH))
    {
        damage = damage * SMALLFISH_SCALE;
    }

    auto targetPlayer = target->ToPlayer();

    if (!targetPlayer)
    {
        return;
    }

    if (!sHardModeHandler->HasMatchingModesWithRestriction(player, targetPlayer, HARDMODE_RESTRICT_BLOCK_CROSSPVP))
    {
        damage = 0;
        sHardModeHandler->SendAlert(player, "You cannot damage players in other modes than your own.");
    }
}
