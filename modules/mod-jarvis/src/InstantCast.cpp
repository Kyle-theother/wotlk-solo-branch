#include "Chat.h"
#include "Item.h"
#include "ItemScript.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellScript.h"
#include "UnitScript.h"

enum InstantCast
{
    ITEM_SCROLL = 900030,
    SPELL_AURA = 900031
};

static bool HasInstantCast(Unit* unit)
{
    return unit && unit->HasAura(SPELL_AURA);
}

class item_instant_cast_scroll : public ItemScript
{
public:
    item_instant_cast_scroll() : ItemScript("item_instant_cast_scroll") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        if (!player->HasSpell(SPELL_AURA))
        {
            player->learnSpell(SPELL_AURA, false);
            ChatHandler(player->GetSession()).SendSysMessage("You have learned Aura of Instant Cast. Click the spell to toggle it.");
            return false;
        }
        if (player->HasAura(SPELL_AURA))
        {
            player->RemoveAura(SPELL_AURA);
            ChatHandler(player->GetSession()).SendSysMessage("Aura of Instant Cast is off.");
        }
        else
        {
            player->CastSpell(player, SPELL_AURA, true);
            ChatHandler(player->GetSession()).SendSysMessage("Aura of Instant Cast is on. Casts are instant and spell effects are halved.");
        }
        return false;
    }
};

class spell_aura_instant_cast : public AuraScript
{
    PrepareAuraScript(spell_aura_instant_cast);

    void HandleApply(AuraEffect const* /*effect*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            ChatHandler(player->GetSession()).SendSysMessage("Aura of Instant Cast is on. Casts are instant and spell effects are halved.");
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_aura_instant_cast::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class instant_cast_effect : public UnitScript
{
public:
    instant_cast_effect() : UnitScript("instant_cast_effect") { }

    void ModifySpellDamageTaken(Unit* /*target*/, Unit* attacker, int32& damage, SpellInfo const* /*spellInfo*/) override
    {
        if (HasInstantCast(attacker))
            damage /= 2;
    }

    void ModifyPeriodicDamageAurasTick(Unit* /*target*/, Unit* attacker, uint32& damage, SpellInfo const* /*spellInfo*/) override
    {
        if (HasInstantCast(attacker))
            damage /= 2;
    }

    void ModifyHealReceived(Unit* /*target*/, Unit* healer, uint32& heal, SpellInfo const* /*spellInfo*/) override
    {
        if (HasInstantCast(healer))
            heal /= 2;
    }
};

void AddJarvisInstantCastScripts()
{
    new item_instant_cast_scroll();
    new instant_cast_effect();
    RegisterSpellScript(spell_aura_instant_cast);
}
