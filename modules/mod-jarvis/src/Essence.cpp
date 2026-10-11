#include "Chat.h"
#include "ItemScript.h"
#include "Map.h"
#include "Player.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "UnitScript.h"
#include "TemporarySummon.h"
#include "Timer.h"
#include <unordered_map>
#include <list>
#include <unordered_set>

constexpr uint32 ESSENCE_ITEM = 900014;
constexpr uint32 JARVIS_TOTEM = 900015;

static uint32 const RAID_BUFFS[] =
{
    25899, // Greater Blessing of Sanctuary
    25898, // Greater Blessing of Kings
    48938, // Greater Blessing of Wisdom
    48934, // Greater Blessing of Might
    43002, // Arcane Brilliance
    47440, // Commanding Shout
    48470, // Gift of the Wild
    48162, // Prayer of Fortitude
    48074, // Prayer of Spirit
    57623, // Horn of Winter
    48170  // Prayer of Shadow Protection
};
static uint32 const RAID_DEBUFFS[] =
{
    47467, // Sunder Armor
    48669, // Expose Armor
    770,   // Faerie Fire
    16857, // Faerie Fire (Feral)
    47437, // Demoralizing Shout
    48560, // Demoralizing Roar
    50511, // Curse of Weakness
    47502, // Thunder Clap
    26016, // Vindication
    47865, // Curse of Elements
    51735, // Ebon Plague
    33198, // Misery
    48564, // Mangle
    47486, // Mortal Strike
    57975, // Wound Poison
    48468, // Insect Swarm
    11719, // Curse of Tongues
    53408, // Judgement of Wisdom
    20271, // Judgement of Light
    53338  // Hunter's Mark
};

static std::unordered_set<uint32> const TRIGGER_SPELLS =
{
    133, 42833, 116, 42842, 30451, 42897, 44614, 47610,
    686, 47809, 29722, 47838, 6353, 47827, 1120, 47855,
    15407, 48156, 8092, 48127, 585, 48123,
    1329, 48666, 1752, 48638, 16511, 48660,
    5221, 48572, 33745, 48568, 5176, 48461, 2912, 48465, 770,
    17364, 51505, 60043, 403, 49238,
    56641, 49052, 53209, 53301, 60053, 53351, 61006,
    49020, 51425, 45902, 49930, 49998, 49924, 45477, 49909, 45462, 49921,
    35395, 53600, 61411, 20473, 48825, 24275, 48806,
    23881, 12294, 47486, 78, 47450, 5308, 47471
};

static std::unordered_map<ObjectGuid, uint32> nextJarvisTotem;

static bool HasEssence(Player* player)
{
    return player && player->HasItemCount(ESSENCE_ITEM, 1);
}

static bool IsTriggerSpell(SpellInfo const* spellInfo)
{
    if (!spellInfo)
        return false;
    if (TRIGGER_SPELLS.contains(spellInfo->Id))
        return true;
    SpellInfo const* first = spellInfo->GetFirstRankSpell();
    return first && TRIGGER_SPELLS.contains(first->Id);
}

static uint32 const TOTEM_BUFFS[] =
{
    58643, // Strength of Earth Totem
    58753, // Stoneskin Totem
    58774, // Mana Spring Totem
    3738,  // Wrath of Air Totem
    58656, // Flametongue Totem
    57663, // Totem of Wrath
    8512   // Windfury Totem
};

static uint32 const COMBAT_TOTEMS[] =
{
    58745, // Frost Resistance Totem
    58741, // Fire Resistance Totem
    58774, // Mana Spring Totem
    58656, // Flametongue Totem
    58749, // Nature Resistance Totem
    8512,  // Windfury Totem
    57663  // Totem of Wrath
};

static void ApplyHourAura(Unit* target, uint32 spell)
{
    Aura* aura = target->AddAura(spell, target);
    if (!aura)
        return;
    aura->SetMaxDuration(60 * MINUTE * IN_MILLISECONDS);
    aura->SetDuration(60 * MINUTE * IN_MILLISECONDS);
}


static void ApplyTotemBuffs(Player* player)
{
    for (uint32 spell : TOTEM_BUFFS)
        ApplyHourAura(player, spell);
    for (uint32 spell : COMBAT_TOTEMS)
        ApplyHourAura(player, spell);
}

static void ApplyControlImmunity(Player* player)
{
    player->RemoveAurasWithMechanic(1ULL << MECHANIC_POLYMORPH);
    player->ApplySpellImmune(ESSENCE_ITEM, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
    for (uint32 spell : { 118u, 12826u, 28271u, 28272u, 61305u, 61721u, 61780u, 51514u })
        player->ApplySpellImmune(ESSENCE_ITEM, IMMUNITY_ID, spell, true);
}

static void ApplyRaidBuffs(Player* player)
{
    for (uint32 spell : RAID_BUFFS)
        ApplyHourAura(player, spell);
    ApplyTotemBuffs(player);
    ApplyControlImmunity(player);
}

static void ApplyRaidDebuffs(Player* player, Unit* victim)
{
    if (!victim || victim == player || !player->IsHostileTo(victim))
        return;
    for (uint32 spell : RAID_DEBUFFS)
        player->AddAura(spell, victim);
}

static void SummonJarvisTotem(Player* player, Unit* target)
{
    uint32 now = getMSTime();
    auto itr = nextJarvisTotem.find(player->GetGUID());
    if (itr != nextJarvisTotem.end() && getMSTimeDiff(itr->second, now) < 20 * IN_MILLISECONDS)
        return;

    Position pos = player->GetPosition();
    if (target && target != player)
        pos = target->GetPosition();
    pos.m_positionX += 2.0f;

    if (Creature* totem = player->SummonCreature(JARVIS_TOTEM, pos, TEMPSUMMON_TIMED_DESPAWN, 60 * MINUTE * IN_MILLISECONDS))
    {
        totem->SetFaction(player->GetFaction());
        nextJarvisTotem[player->GetGUID()] = now;
    }
}

class item_essence_of_immortals : public ItemScript
{
public:
    item_essence_of_immortals() : ItemScript("item_essence_of_immortals") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        ApplyRaidBuffs(player);
        ChatHandler(player->GetSession()).SendSysMessage("Essence of the Immortals: raid and totem buffs applied.");
        return false;
    }
};

class npc_jarvis_totem : public CreatureScript
{
public:
    npc_jarvis_totem() : CreatureScript("npc_jarvis_totem") { }

    struct npc_jarvis_totemAI : public ScriptedAI
    {
        explicit npc_jarvis_totemAI(Creature* creature) : ScriptedAI(creature), timer(2000) { }

        void UpdateAI(uint32 diff) override
        {
            if (timer <= diff)
            {
                static uint32 const spells[] = {
                    58643, 58753, 58774, 3738, 58656, 57663, 8512,
                    58745, 58741, 58749, 48470
                };
                for (auto const& ref : me->GetMap()->GetPlayers())
                {
                    Player* player = ref.GetSource();
                    if (!player || me->GetDistance(player) > 40.0f)
                        continue;
                    player->RemoveAurasDueToSpell(8143);
                    player->RemoveAurasDueToSpell(58764);
                    player->RemoveAurasDueToSpell(8170);
                    for (uint32 spell : spells)
                        ApplyHourAura(player, spell);
                    if (HasEssence(player))
                        ApplyControlImmunity(player);
                    me->CastSpell(player, 52042, true);
                }
                timer = 10000;
            }
            else
                timer -= diff;
        }

        uint32 timer;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_jarvis_totemAI(creature);
    }
};

class essence_player : public PlayerScript
{
public:
    essence_player() : PlayerScript("essence_player") { }

    void OnPlayerMapChanged(Player* player) override
    {
        if (player->GetMap() && player->GetMap()->IsRaid())
            ApplyHourAura(player, 25899);
        if (HasEssence(player) && player->GetMap() && (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid()))
            ApplyRaidBuffs(player);
    }

    void OnPlayerSpellCast(Player* player, Spell* spell, bool /*skipCheck*/) override
    {
        if (!HasEssence(player) || !player->GetMap() || !(player->GetMap()->IsDungeon() || player->GetMap()->IsRaid()))
            return;
        if (!spell || !IsTriggerSpell(spell->GetSpellInfo()))
            return;

        Unit* target = spell->m_targets.GetUnitTarget();
        if (!target)
            target = player->GetSelectedUnit();
        ApplyRaidDebuffs(player, target);
        ApplyTotemBuffs(player);
    }
};


class essence_leech : public UnitScript
{
public:
    essence_leech() : UnitScript("essence_leech") { }

    void OnDamage(Unit* attacker, Unit* victim, uint32& damage) override
    {
        if (!attacker || !victim || attacker == victim || !damage)
            return;

        if (Player* defender = victim->GetCharmerOrOwnerPlayerOrPlayerItself())
            if (defender->GetMap() && defender->GetMap()->IsRaid())
                damage = damage * 70 / 100;

        Player* player = attacker->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;
        if (player->GetMap() && player->GetMap()->IsRaid())
            damage = damage * 35 / 10;

        if (!HasEssence(player) || player->IsFriendlyTo(victim))
            return;

        uint32 heal = damage / 2;
        if (!heal)
            return;

        int32 bp = int32(heal);
        player->CastCustomSpell(player, 15290, &bp, nullptr, nullptr, true);
    }
};

void AddJarvisItemScripts()
{
    new item_essence_of_immortals();
    new npc_jarvis_totem();
    new essence_player();
    new essence_leech();
}
