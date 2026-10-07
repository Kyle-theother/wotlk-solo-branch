#include "Chat.h"
#include "ItemScript.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "Timer.h"
#include "UnitScript.h"
#include <unordered_map>

static std::unordered_map<ObjectGuid, uint32> essenceArmedAt;
static std::unordered_map<ObjectGuid, uint32> essenceNextDebuff;

static void ApplyEssenceBuffs(Player* player)
{
    uint32 const buffs[] = { 48470, 48162, 48074, 43002, 48934, 48938, 25898, 47436 };
    for (uint32 spell : buffs)
        player->AddAura(spell, player);
    essenceArmedAt[player->GetGUID()] = getMSTime();
    ChatHandler(player->GetSession()).SendSysMessage("Essence of Immortals: raid buffs applied.");
}

class item_essence_of_immortals : public ItemScript
{
public:
    item_essence_of_immortals() : ItemScript("item_essence_of_immortals") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        ApplyEssenceBuffs(player);
        return false;
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

        uint32 now = getMSTime();
        auto armed = essenceArmedAt.find(player->GetGUID());
        if (armed == essenceArmedAt.end() || getMSTimeDiff(armed->second, now) > 60 * MINUTE * IN_MILLISECONDS)
            return;

        auto next = essenceNextDebuff.find(player->GetGUID());
        if (next != essenceNextDebuff.end() && getMSTimeDiff(next->second, now) < 8 * IN_MILLISECONDS)
            return;

        uint32 const debuffs[] = { 47467, 47865, 770, 47437, 47486, 53338, 48564 };
        for (uint32 spell : debuffs)
            player->AddAura(spell, victim);
        essenceNextDebuff[player->GetGUID()] = now;
    }
};

void AddJarvisItemScripts()
{
    new item_essence_of_immortals();
    new essence_attack_debuffs();
}
