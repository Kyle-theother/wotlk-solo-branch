#include "Chat.h"
#include "ItemScript.h"
#include "Map.h"
#include "Player.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "TemporarySummon.h"
#include "Timer.h"
#include <unordered_map>
#include <unordered_set>

constexpr uint32 ESSENCE_ITEM = 900014;
constexpr uint32 ROY_TOTEM = 900015;

static uint32 const RAID_BUFFS[] = { 48470, 48162, 48074, 43002, 48934, 48938, 25898, 47436 };
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

static std::unordered_map<ObjectGuid, uint32> nextRoyTotem;

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

static void SummonRoy(Player* player, Unit* target)
{
    uint32 now = getMSTime();
    auto itr = nextRoyTotem.find(player->GetGUID());
    if (itr != nextRoyTotem.end() && getMSTimeDiff(itr->second, now) < 20 * IN_MILLISECONDS)
        return;

    Position pos = target && target != player ? target->GetPosition() : player->GetNearPosition(3.0f, 0.0f);
    if (Creature* totem = player->SummonCreature(ROY_TOTEM, pos, TEMPSUMMON_TIMED_DESPAWN, 30 * IN_MILLISECONDS))
    {
        totem->SetFaction(player->GetFaction());
        totem->SetOwnerGUID(player->GetGUID());
        nextRoyTotem[player->GetGUID()] = now;
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

class npc_roy_totem : public CreatureScript
{
public:
    npc_roy_totem() : CreatureScript("npc_roy_totem") { }

    struct npc_roy_totemAI : public ScriptedAI
    {
        npc_roy_totemAI(Creature* creature) : ScriptedAI(creature), timer(2000) { }

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
        return new npc_roy_totemAI(creature);
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
        SummonRoy(player, target);
    }
};

void AddJarvisItemScripts()
{
    new item_essence_of_immortals();
    new npc_roy_totem();
    new essence_player();
}
