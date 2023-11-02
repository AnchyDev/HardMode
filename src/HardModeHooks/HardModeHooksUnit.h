#ifndef MODULE_HARDMODE_HOOKS_UNIT_H
#define MODULE_HARDMODE_HOOKS_UNIT_H

#include "ScriptMgr.h"

class HardModeHooksUnitScript : UnitScript
{
public:
    HardModeHooksUnitScript() : UnitScript("HardModeHooksUnitScript") { }

private:
    void OnAuraApply(Unit* unit, Aura* aura) override;
    void OnAuraRemove(Unit* unit, AuraApplication* auraApp, AuraRemoveMode mode) override;

    void OnDamage(Unit* attacker, Unit* victim, uint32& damage) override;
    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* spellInfo) override;
    void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override;
    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* spellInfo) override;

    void OnUnitDeath(Unit* unit, Unit* killer) override;
};

#endif // MODULE_HARDMODE_HOOKS_UNIT_H
