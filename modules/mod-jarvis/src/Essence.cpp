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
#include "TemporarySummon.h"
#include "Timer.h"
#include <unordered_map>
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
    57623  // Horn of Winter
};
static uint32 const RAID_DEBUFFS[] = { 47467, 47865, 770, 47437, 47486, 53338, 48564 };

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

static void ApplyRaidBuffs(Player* player)
{
    for (uint32 spell : RAID_BUFFS)
        player->AddAura(spell, player);
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

    if (Creature* totem = player->SummonCreature(JARVIS_TOTEM, pos, TEMPSUMMON_TIMED_DESPAWN, 30 * IN_MILLISECONDS))
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
        ChatHandler(player->GetSession()).SendSysMessage("Essence of the Immortals: raid buffs applied.");
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
                me->CastSpell(me, 48470, true);
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
        if (HasEssence(player) && player->GetMap() && player->GetMap()->IsDungeon())
            ApplyRaidBuffs(player);
    }

    void OnPlayerSpellCast(Player* player, Spell* spell, bool /*skipCheck*/) override
    {
        if (!HasEssence(player) || !player->GetMap() || !player->GetMap()->IsDungeon())
            return;
        if (!spell || !IsTriggerSpell(spell->GetSpellInfo()))
            return;

        Unit* target = spell->m_targets.GetUnitTarget();
        if (!target)
            target = player->GetSelectedUnit();
        ApplyRaidDebuffs(player, target);
        SummonJarvisTotem(player, target);
    }
};

void AddJarvisItemScripts()
{
    new item_essence_of_immortals();
    new npc_jarvis_totem();
    new essence_player();
}
