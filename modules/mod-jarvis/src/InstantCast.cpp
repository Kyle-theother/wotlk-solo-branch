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

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (player->HasSpell(SPELL_AURA))
        {
            ChatHandler(player->GetSession()).SendSysMessage("You already know Aura of Instant Cast.");
            return false;
        }
        player->learnSpell(SPELL_AURA, false);
        player->DestroyItemCount(item->GetEntry(), 1, true);
        ChatHandler(player->GetSession()).SendSysMessage("You have learned Aura of Instant Cast. Cast the spell to turn it on, and cancel the aura to turn it off.");
        return false;
    }
};

class spell_instant_cast_toggle : public SpellScript
{
    PrepareSpellScript(spell_instant_cast_toggle);

    SpellCastResult CheckCast()
    {
        if (GetCaster() && GetCaster()->HasAura(SPELL_AURA))
        {
            GetCaster()->RemoveAura(SPELL_AURA);
            if (Player* player = GetCaster()->ToPlayer())
                ChatHandler(player->GetSession()).SendSysMessage("Aura of Instant Cast is off.");
            return SPELL_FAILED_DONT_REPORT;
        }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_instant_cast_toggle::CheckCast);
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
    RegisterSpellScript(spell_instant_cast_toggle);
}
