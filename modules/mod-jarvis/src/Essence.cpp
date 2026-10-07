#include "ItemScript.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "Timer.h"
#include "UnitScript.h"
#include <unordered_map>

static std::unordered_map<ObjectGuid, uint32> essenceArmedUntil;
static std::unordered_map<ObjectGuid, uint32> essenceNextDebuff;

class item_essence_of_immortals : public ItemScript
{
public:
    item_essence_of_immortals() : ItemScript("item_essence_of_immortals") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        uint32 const buffs[] = { 48470, 48162, 48074, 43002, 48934, 48938, 25898, 47436 };
        for (uint32 spell : buffs)
            player->AddAura(spell, player);
        essenceArmedUntil[player->GetGUID()] = getMSTime() + 60 * MINUTE * IN_MILLISECONDS;
        return true;
    }
};

class essence_attack_debuffs : public UnitScript
{
public:
    essence_attack_debuffs() : UnitScript("essence_attack_debuffs") { }

    void OnDamage(Unit* attacker, Unit* victim, uint32& /*damage*/) override
    {
        Player* player = attacker ? attacker->ToPlayer() : nullptr;
        if (!player || !victim || victim == player || !player->IsHostileTo(victim))
            return;

        auto armed = essenceArmedUntil.find(player->GetGUID());
        if (armed == essenceArmedUntil.end() || getMSTimeDiff(getMSTime(), armed->second) == 0)
            return;

        uint32 now = getMSTime();
        auto next = essenceNextDebuff.find(player->GetGUID());
        if (next != essenceNextDebuff.end() && getMSTimeDiff(next->second, now) > 8 * IN_MILLISECONDS)
            return;

        uint32 const debuffs[] =
        {
            47467, // Sunder Armor
            47865, // Curse of Elements
            770,   // Faerie Fire
            47437, // Demoralizing Shout
            47486, // Mortal Strike
            53338, // Hunter's Mark
            48564  // Mangle
        };
        for (uint32 spell : debuffs)
            player->AddAura(spell, victim);
        essenceNextDebuff[player->GetGUID()] = now + 8 * IN_MILLISECONDS;
    }
};

void AddJarvisItemScripts()
{
    new item_essence_of_immortals();
    new essence_attack_debuffs();
}
