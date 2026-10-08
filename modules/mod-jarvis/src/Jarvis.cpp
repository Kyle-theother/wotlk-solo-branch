#include "Chat.h"
#include "CreatureScript.h"
#include "DatabaseEnv.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "JarvisSpells.inc"
#include <string>

enum JarvisAction
{
    ACT_SALE = 1, ACT_PROF = 2, ACT_REP = 3, ACT_RESET = 4, ACT_HEIRLOOM = 5,
    ACT_TRAVEL = 6, ACT_CITIES = 7, ACT_RAIDS = 8, ACT_ZONES = 9,
    ACT_RACIAL = 20, ACT_TALENT = 21, ACT_SPELLS = 22, ACT_GEAR = 23, ACT_SPELL_CLASS = 24, ACT_BACK = 99
};

static uint8 const gearLevels[] = { 1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80 };
static uint32 GearCost(uint8 level) { return level <= 1 ? 1 : uint32(level) * 100; }
static std::string GearPrice(uint8 level) { return level <= 1 ? "1 copper" : std::to_string(level) + " silver"; }
static uint8 ArmorFor(Player* player, uint8 level)
{
    switch (player->getClass())
    {
        case CLASS_ROGUE: case CLASS_DRUID: return 2;
        case CLASS_HUNTER: return 3;
        case CLASS_SHAMAN: return level >= 40 ? 3 : 2;
        case CLASS_WARRIOR: case CLASS_PALADIN: return level >= 40 ? 4 : 3;
        case CLASS_DEATH_KNIGHT: return 4;
        default: return 1;
    }
}
static bool TakeCopper(Player* player, uint32 copper)
{
    if (player->GetMoney() < copper)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You do not have enough money.");
        return false;
    }
    player->ModifyMoney(-int32(copper));
    return true;
}
static void GiveItem(Player* player, uint32 entry)
{
    if (entry)
        player->AddItem(entry, 1);
}
static void GiveLevelGreens(Player* player, uint8 level)
{
    uint8 armor = ArmorFor(player, level);
    uint8 floor = level <= 5 ? 0 : level - 5;
    uint32 given = 0;
    bool haveSlot[17] = {};
    if (QueryResult armorRows = WorldDatabase.Query("SELECT InventoryType, MAX(entry) FROM item_template WHERE Quality = 2 AND class = 4 AND subclass = {} AND InventoryType IN (1,3,5,6,7,8,9,10) AND RequiredLevel BETWEEN {} AND {} AND (AllowableClass = -1 OR AllowableClass & {}) GROUP BY InventoryType ORDER BY MAX(RequiredLevel) DESC LIMIT 8", armor, floor, level, player->getClassMask()))
    {
        do
        {
            uint8 slot = armorRows->Fetch()[0].Get<uint8>();
            if (slot < 17)
                haveSlot[slot] = true;
            GiveItem(player, armorRows->Fetch()[1].Get<uint32>());
            ++given;
        } while (armorRows->NextRow());
    }
    bool haveWeapon = false;
    if (QueryResult weapon = WorldDatabase.Query("SELECT entry FROM item_template WHERE Quality = 2 AND class = 2 AND RequiredLevel BETWEEN {} AND {} AND (AllowableClass = -1 OR AllowableClass & {}) ORDER BY RequiredLevel DESC LIMIT 1", floor, level, player->getClassMask()))
    {
        GiveItem(player, weapon->Fetch()[0].Get<uint32>());
        haveWeapon = true;
        ++given;
    }
    if (level <= 1)
    {
        auto whiteArmor = [&](uint8 slot)
        {
            if (haveSlot[slot])
                return;
            if (QueryResult row = WorldDatabase.Query("SELECT entry FROM item_template WHERE Quality = 1 AND class = 4 AND subclass = {} AND InventoryType = {} AND RequiredLevel <= 1 AND (AllowableClass = -1 OR AllowableClass & {}) ORDER BY ItemLevel DESC LIMIT 1", armor, slot, player->getClassMask()))
            {
                GiveItem(player, row->Fetch()[0].Get<uint32>());
                ++given;
            }
        };
        for (uint8 slot : { uint8(1), uint8(3), uint8(5), uint8(6), uint8(7), uint8(8), uint8(9), uint8(10) })
            whiteArmor(slot);
        auto whiteMisc = [&](uint8 slot, uint8 subclass, uint32 count)
        {
            if (QueryResult row = WorldDatabase.Query("SELECT entry FROM item_template WHERE Quality = 1 AND class = 4 AND subclass = {} AND InventoryType = {} AND RequiredLevel <= 1 ORDER BY ItemLevel DESC LIMIT 1", subclass, slot))
                for (uint32 i = 0; i < count; ++i)
                {
                    GiveItem(player, row->Fetch()[0].Get<uint32>());
                    ++given;
                }
        };
        whiteMisc(2, 0, 1);
        whiteMisc(16, 1, 1);
        whiteMisc(11, 0, 2);
        whiteMisc(12, 0, 2);
        if (!haveWeapon)
            if (QueryResult row = WorldDatabase.Query("SELECT entry FROM item_template WHERE Quality = 1 AND class = 2 AND RequiredLevel <= 1 AND (AllowableClass = -1 OR AllowableClass & {}) ORDER BY ItemLevel DESC LIMIT 1", player->getClassMask()))
            {
                GiveItem(player, row->Fetch()[0].Get<uint32>());
                ++given;
            }
    }
    if (!given) ChatHandler(player->GetSession()).SendSysMessage("No gear was found for that level.");
}

struct JarvisDest { char const* name; uint32 map; float x, y, z, o; };
struct JarvisFaction { char const* name; uint32 id; uint8 team; uint32 gold; };
struct JarvisProf { char const* name; uint32 skill; uint16 scale; uint32 tool; uint32 spells[6]; };

static JarvisDest const cities[] =
{
    { "Stormwind", 0, -8832.0f, 628.0f, 94.0f, 0.7f }, { "Ironforge", 0, -4920.0f, -946.0f, 502.0f, 5.4f },
    { "Darnassus", 1, 9866.0f, 2494.0f, 1316.0f, 5.5f }, { "The Exodar", 530, -3965.0f, -11653.0f, -138.0f, 5.6f },
    { "Orgrimmar", 1, 1569.0f, -4420.0f, 16.0f, 0.0f }, { "Thunder Bluff", 1, -1277.0f, 124.0f, 131.0f, 4.6f },
    { "Undercity", 0, 1632.0f, 240.0f, -43.0f, 6.2f }, { "Silvermoon", 530, 9470.0f, -7278.0f, 14.0f, 6.1f },
    { "Shattrath", 530, -1833.0f, 5300.0f, -12.0f, 2.0f }, { "Dalaran", 571, 5804.0f, 556.0f, 651.0f, 1.5f }
};
static JarvisDest const raids[] =
{
    { "Naxxramas", 571, 3668.0f, -1262.0f, 243.0f, 5.0f }, { "Ulduar", 571, 9345.0f, -1114.0f, 1245.0f, 5.7f },
    { "Icecrown Citadel", 571, 5873.0f, 2110.0f, 636.0f, 1.5f }, { "Onyxia's Lair", 1, -4707.0f, -3726.0f, 54.0f, 3.4f },
    { "Molten Core", 0, -7523.0f, -1228.0f, 285.0f, 4.5f }, { "Karazhan", 0, -11118.0f, -2011.0f, 47.0f, 0.6f },
    { "Black Temple", 530, -3644.0f, 316.0f, 35.0f, 2.9f }
};
static JarvisDest const zones[] =
{
    { "Stranglethorn Vale", 0, -14302.0f, 518.0f, 8.0f, 2.6f }, { "Silithus", 1, -6812.0f, 834.0f, 50.0f, 4.7f }
};
static JarvisFaction const factions[] =
{
    { "Stormwind", 72, 1, 200 }, { "Ironforge", 47, 1, 200 }, { "Darnassus", 69, 1, 200 }, { "Exodar", 930, 1, 400 }, { "Gnomeregan", 54, 1, 300 },
    { "Orgrimmar", 76, 2, 200 }, { "Thunder Bluff", 81, 2, 200 }, { "Undercity", 68, 2, 200 }, { "Silvermoon", 911, 2, 400 }, { "Darkspear Trolls", 530, 2, 300 },
    { "Cenarion Circle", 609, 0, 800 }, { "Argent Crusade", 1106, 0, 1500 }, { "Kirin Tor", 1090, 0, 1500 },
    { "Knights of the Ebon Blade", 1098, 0, 2000 }, { "Wyrmrest Accord", 1091, 0, 1500 }
};
static JarvisProf const profs[] =
{
    { "Mining", 186, 50, 2901, { 2575, 2576, 3564, 10248, 29354, 50310 } },
    { "Herbalism", 182, 50, 0, { 2366, 2368, 3570, 11993, 28695, 50300 } },
    { "Skinning", 393, 60, 7005, { 8613, 8617, 8618, 10768, 32678, 50305 } },
    { "Fishing", 356, 60, 6256, { 7620, 7731, 7732, 18248, 33095, 51294 } },
    { "First Aid", 129, 60, 0, { 3273, 3274, 7924, 10846, 27028, 45542 } },
    { "Cooking", 185, 70, 0, { 2550, 3102, 3413, 18260, 33359, 51296 } },
    { "Alchemy", 171, 100, 0, { 2259, 3101, 3464, 11611, 28596, 51304 } },
    { "Tailoring", 197, 100, 0, { 3908, 3909, 3910, 12180, 26790, 51309 } },
    { "Leatherworking", 165, 110, 0, { 2108, 3104, 3811, 10662, 32549, 51302 } },
    { "Jewelcrafting", 755, 120, 20815, { 25229, 25230, 28894, 28895, 28897, 51311 } },
    { "Inscription", 773, 120, 39505, { 45357, 45358, 45359, 45360, 45361, 45363 } },
    { "Enchanting", 333, 140, 6218, { 7411, 7412, 7413, 13920, 28029, 51313 } },
    { "Engineering", 202, 150, 6219, { 4036, 4037, 4038, 12656, 30350, 51306 } },
    { "Blacksmithing", 164, 160, 5956, { 2018, 3100, 3538, 9785, 29844, 51300 } }
};
static JarvisSpell const racials[] =
{
    { "Human racials", 59752, 200, 0 }, { "Dwarf racials", 20594, 200, 0 }, { "Night elf racials", 20580, 200, 0 },
    { "Gnome racials", 20589, 200, 0 }, { "Draenei racials", 28880, 200, 0 }, { "Orc racials", 20572, 200, 1 },
    { "Undead racials", 7744, 200, 0 }, { "Tauren racials", 20549, 200, 0 }, { "Troll racials", 26297, 200, 0 }, { "Blood elf racials", 28730, 200, 0 }
};
static uint32 const racialExtra[][4] =
{
    { 58985, 20597, 20598, 0 }, { 2481, 20595, 20596, 0 }, { 20582, 20583, 20585 }, { 20591, 20592, 20593, 0 }, { 28875, 6562, 0, 0 },
    { 20573, 21563, 0, 0 }, { 20577, 20579, 5227, 0 }, { 20550, 20551, 20552, 0 }, { 20555, 20557, 26290, 0 }, { 28877, 822, 0, 0 }
};

static uint32 ProfCost(uint16 scale, uint8 tier) { uint32 base = tier == 1 ? 50 : (tier == 2 ? 750 : 1500); return base * scale / 100; }
static uint16 ProfMax(uint8 tier) { return tier == 1 ? 300 : (tier == 2 ? 375 : 450); }
static char const* TierName(uint8 tier) { return tier == 1 ? "Vanilla" : (tier == 2 ? "TBC" : "Wrath"); }
static bool KnowsRacial(Player* player, uint32 index)
{
    if (player->HasSpell(racials[index].id)) return true;
    for (uint32 spell : racialExtra[index]) if (spell && player->HasSpell(spell)) return true;
    return false;
}
static bool TakeGold(Player* player, uint32 gold)
{
    if (player->GetMoney() < gold * GOLD) { ChatHandler(player->GetSession()).SendSysMessage("You do not have enough gold."); return false; }
    player->ModifyMoney(-int32(gold * GOLD));
    return true;
}
static void LearnTrainerRecipes(Player* player, uint32 skill, uint16 maxRank)
{
    if (QueryResult result = WorldDatabase.Query("SELECT DISTINCT SpellID FROM npc_trainer WHERE ReqSkillLine = {} AND ReqSkillRank <= {} AND SpellID > 0", skill, maxRank))
        do { uint32 spell = result->Fetch()[0].Get<uint32>(); if (!player->HasSpell(spell)) player->learnSpell(spell, false); } while (result->NextRow());
    if (QueryResult vendor = WorldDatabase.Query("SELECT DISTINCT item.spellid_1 FROM npc_vendor vendor JOIN item_template item ON item.entry = vendor.item WHERE item.class = 9 AND item.spellid_1 > 0 AND item.RequiredSkill = {} AND item.RequiredSkillRank <= {}", skill, maxRank))
        do { uint32 spell = vendor->Fetch()[0].Get<uint32>(); if (!player->HasSpell(spell)) player->learnSpell(spell, false); } while (vendor->NextRow());
}
static void TeachProfession(Player* player, uint32 index, uint8 tier)
{
    JarvisProf const& prof = profs[index];
    uint32 count = tier == 1 ? 4 : (tier == 2 ? 5 : 6);
    for (uint32 i = 0; i < count; ++i) player->learnSpell(prof.spells[i], false);
    player->SetSkill(prof.skill, 1, ProfMax(tier), ProfMax(tier));
    if (prof.tool) player->AddItem(prof.tool, 1);
    if (prof.skill == 333) player->AddItem(tier == 1 ? 6218 : (tier == 2 ? 22463 : 44452), 1);
    LearnTrainerRecipes(player, prof.skill, ProfMax(tier));
}
static void GiveReputation(Player* player, FactionEntry const* entry)
{
    ReputationMgr& mgr = player->GetReputationMgr();
    mgr.SetOneFactionReputation(entry, 42000, false);
    mgr.SendInitialReputations();
}
static void ShowClassSpells(Player* player, uint32 classIndex)
{
    uint8 cls = spellClassIds[classIndex];
    for (uint32 i = 0; i < sizeof(borrowed) / sizeof(borrowed[0]); ++i)
        if (borrowed[i].cls == cls && !player->HasSpell(borrowed[i].id))
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, std::string(borrowed[i].name) + " - " + std::to_string(borrowed[i].gold) + "g", ACT_SPELL_CLASS, i, "Learn this ability?", borrowed[i].gold * GOLD, false);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_SPELLS);
}

class npc_jarvis : public CreatureScript
{
public:
    npc_jarvis() : CreatureScript("npc_jarvis") { }
    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "What do you have for sale?", GOSSIP_SENDER_MAIN, ACT_SALE);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Buy green gear for a level", GOSSIP_SENDER_MAIN, ACT_GEAR);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Learn professions", GOSSIP_SENDER_MAIN, ACT_PROF);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Can you help me gain reputation?", GOSSIP_SENDER_MAIN, ACT_REP);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Learn another race's racials", GOSSIP_SENDER_MAIN, ACT_RACIAL);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Buy extra talent points", GOSSIP_SENDER_MAIN, ACT_TALENT);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Learn another class's abilities", GOSSIP_SENDER_MAIN, ACT_SPELLS);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "I'd like to reset all instances", GOSSIP_SENDER_MAIN, ACT_RESET, "Reset all instance locks for 1 gold?", GOLD, false);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "I'd like to purchase a class heirloom set", GOSSIP_SENDER_MAIN, ACT_HEIRLOOM);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Where can you take me?", GOSSIP_SENDER_MAIN, ACT_TRAVEL);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Nevermind", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (sender == ACT_CITIES && action < sizeof(cities) / sizeof(cities[0])) { CloseGossipMenuFor(player); player->TeleportTo(cities[action].map, cities[action].x, cities[action].y, cities[action].z, cities[action].o); return true; }
        if (sender == ACT_RAIDS && action < sizeof(raids) / sizeof(raids[0])) { CloseGossipMenuFor(player); player->TeleportTo(raids[action].map, raids[action].x, raids[action].y, raids[action].z, raids[action].o); return true; }
        if (sender == ACT_ZONES && action < sizeof(zones) / sizeof(zones[0])) { CloseGossipMenuFor(player); player->TeleportTo(zones[action].map, zones[action].x, zones[action].y, zones[action].z, zones[action].o); return true; }
        if (sender == ACT_GEAR)
        {
            for (uint8 level : gearLevels) if (level == action) { CloseGossipMenuFor(player); if (TakeCopper(player, GearCost(level))) GiveLevelGreens(player, level); return true; }
        }
        if (sender == ACT_REP)
        {
            uint32 seen = 0; uint8 team = player->GetTeamId() == TEAM_ALLIANCE ? 1 : 2;
            for (JarvisFaction const& faction : factions)
            {
                if (faction.team != 0 && faction.team != team) continue;
                if (seen == action) { CloseGossipMenuFor(player); if (TakeGold(player, faction.gold)) if (FactionEntry const* entry = sFactionStore.LookupEntry(faction.id)) GiveReputation(player, entry); return true; }
                ++seen;
            }
        }
        if (sender == ACT_RACIAL && action < sizeof(racials) / sizeof(racials[0]) && !KnowsRacial(player, action))
        {
            CloseGossipMenuFor(player);
            if (TakeGold(player, racials[action].gold)) { player->learnSpell(racials[action].id, false); for (uint32 spell : racialExtra[action]) if (spell) player->learnSpell(spell, false); }
            return true;
        }
        if (sender == ACT_SPELL_CLASS && action < sizeof(borrowed) / sizeof(borrowed[0]) && !player->HasSpell(borrowed[action].id))
        {
            CloseGossipMenuFor(player);
            if (TakeGold(player, borrowed[action].gold)) player->learnSpell(borrowed[action].id, false);
            return true;
        }
        if (sender == ACT_TALENT && (action == 1 || action == 5 || action == 10))
        {
            CloseGossipMenuFor(player);
            if (TakeGold(player, action * 100)) { player->RewardExtraBonusTalentPoints(action); player->SetFreeTalentPoints(player->GetFreeTalentPoints() + action); player->SendTalentsInfoData(false); }
            return true;
        }
        if (sender == ACT_PROF && action >= 10)
        {
            uint32 index = action / 10 - 1; uint8 tier = action % 10;
            if (index < sizeof(profs) / sizeof(profs[0]) && tier >= 1 && tier <= 3 && player->GetSkillValue(profs[index].skill) < ProfMax(tier))
            {
                CloseGossipMenuFor(player);
                if (TakeGold(player, ProfCost(profs[index].scale, tier))) TeachProfession(player, index, tier);
                return true;
            }
        }
        if (sender == ACT_HEIRLOOM)
        {
            uint32 items[8] = {}; uint32 count = 0;
            auto add = [&](std::initializer_list<uint32> list) { for (uint32 item : list) if (count < 8) items[count++] = item; };
            if (action == 11) add({ 42949, 48685, 42945, 48716, 42991, 42992, 50255 });
            else if (action == 12) add({ 42949, 48685, 42943, 44092, 42991, 42992, 50255 });
            else if (action == 21) add({ 42949, 48685, 42945, 44094, 42992, 42991, 50255 });
            else if (action == 22) add({ 42949, 48685, 44092, 48718, 42991, 42992, 50255 });
            else if (action == 31) add({ 42950, 48677, 42946, 44093, 42991, 42992, 50255 });
            else if (action == 41) add({ 42952, 48689, 42944, 44091, 42991, 42992, 50255 });
            else if (action == 51) add({ 42985, 48691, 42947, 42948, 42992, 42991, 50255 });
            else if (action == 61) add({ 42949, 48685, 42943, 42945, 42991, 42992, 50255 });
            else if (action == 71) add({ 42950, 48677, 42948, 44094, 42992, 42991, 50255 });
            else if (action == 81) add({ 42985, 48691, 42947, 44095, 42992, 42991, 50255 });
            else if (action == 91) add({ 42985, 48691, 42947, 44095, 42992, 42991, 50255 });
            else if (action == 111) add({ 42952, 48689, 42947, 48718, 42992, 42991, 50255 });
            if (count) { CloseGossipMenuFor(player); if (TakeGold(player, 1)) for (uint32 i = 0; i < count; ++i) player->AddItem(items[i], 1); return true; }
        }
        ClearGossipMenuFor(player);
        if (action == ACT_BACK) return OnGossipHello(player, creature);
        if (action >= 400 && action < 410) { ShowClassSpells(player, action - 400); SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID()); return true; }
        switch (action)
        {
            case ACT_SALE: player->GetSession()->SendListInventory(creature->GetGUID()); return true;
            case ACT_RESET:
                CloseGossipMenuFor(player);
                if (TakeGold(player, 1)) { Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_ALL, false); Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_ALL, true); }
                return true;
            case ACT_GEAR:
                for (uint8 level : gearLevels) AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Level " + std::to_string(level) + " greens - " + GearPrice(level), ACT_GEAR, level, "Buy this green set?", GearCost(level), false);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            case ACT_PROF:
                for (uint32 i = 0; i < sizeof(profs) / sizeof(profs[0]); ++i) if (player->GetSkillValue(profs[i].skill) < 450) AddGossipItemFor(player, GOSSIP_ICON_TRAINER, profs[i].name, GOSSIP_SENDER_MAIN, 200 + i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            case ACT_REP:
            {
                uint8 team = player->GetTeamId() == TEAM_ALLIANCE ? 1 : 2; uint32 shown = 0;
                for (JarvisFaction const& faction : factions)
                {
                    if (faction.team != 0 && faction.team != team) continue;
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(faction.name) + " - " + std::to_string(faction.gold) + "g", ACT_REP, shown, "Buy exalted reputation?", faction.gold * GOLD, false);
                    ++shown;
                }
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            }
            case ACT_RACIAL:
                for (uint32 i = 0; i < sizeof(racials) / sizeof(racials[0]); ++i) if (!KnowsRacial(player, i)) AddGossipItemFor(player, GOSSIP_ICON_TRAINER, std::string(racials[i].name) + " - 200g", ACT_RACIAL, i, "Learn these racials?", 200 * GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            case ACT_TALENT:
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "1 talent point - 100g", ACT_TALENT, 1, "Buy 1 talent point?", 100 * GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "5 talent points - 500g", ACT_TALENT, 5, "Buy 5 talent points?", 500 * GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "10 talent points - 1000g", ACT_TALENT, 10, "Buy 10 talent points?", 1000 * GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            case ACT_SPELLS:
                for (uint32 i = 0; i < sizeof(spellClassIds) / sizeof(spellClassIds[0]); ++i)
                {
                    bool any = false;
                    for (JarvisSpell const& spell : borrowed) if (spell.cls == spellClassIds[i] && !player->HasSpell(spell.id)) any = true;
                    if (any) AddGossipItemFor(player, GOSSIP_ICON_TRAINER, spellClasses[i], GOSSIP_SENDER_MAIN, 400 + i);
                }
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            case ACT_HEIRLOOM:
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Warrior, one-hand", ACT_HEIRLOOM, 11, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Warrior, two-hand", ACT_HEIRLOOM, 12, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Paladin, one-hand", ACT_HEIRLOOM, 21, "Buy this set for 1 gold? No heirloom shield exists in Wrath.", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Paladin, two-hand", ACT_HEIRLOOM, 22, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Hunter", ACT_HEIRLOOM, 31, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Rogue", ACT_HEIRLOOM, 41, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Priest", ACT_HEIRLOOM, 51, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Death Knight", ACT_HEIRLOOM, 61, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Shaman", ACT_HEIRLOOM, 71, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Mage", ACT_HEIRLOOM, 81, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Warlock", ACT_HEIRLOOM, 91, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Druid", ACT_HEIRLOOM, 111, "Buy this set for 1 gold?", GOLD, false);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            case ACT_TRAVEL:
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Cities", GOSSIP_SENDER_MAIN, ACT_CITIES);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Raids", GOSSIP_SENDER_MAIN, ACT_RAIDS);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Stranglethorn and Silithus", GOSSIP_SENDER_MAIN, ACT_ZONES);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK); break;
            case ACT_CITIES:
                for (uint32 i = 0; i < sizeof(cities) / sizeof(cities[0]); ++i) AddGossipItemFor(player, GOSSIP_ICON_TAXI, cities[i].name, ACT_CITIES, i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_TRAVEL); break;
            case ACT_RAIDS:
                for (uint32 i = 0; i < sizeof(raids) / sizeof(raids[0]); ++i) AddGossipItemFor(player, GOSSIP_ICON_TAXI, raids[i].name, ACT_RAIDS, i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_TRAVEL); break;
            case ACT_ZONES:
                for (uint32 i = 0; i < sizeof(zones) / sizeof(zones[0]); ++i) AddGossipItemFor(player, GOSSIP_ICON_TAXI, zones[i].name, ACT_ZONES, i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_TRAVEL); break;
            default:
                if (action >= 200 && action < 200 + sizeof(profs) / sizeof(profs[0]))
                {
                    uint32 index = action - 200;
                    for (uint8 tier = 1; tier <= 3; ++tier)
                        if (player->GetSkillValue(profs[index].skill) < ProfMax(tier))
                            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, std::string(TierName(tier)) + " " + profs[index].name + " - " + std::to_string(ProfCost(profs[index].scale, tier)) + "g", ACT_PROF, (index + 1) * 10 + tier, "Learn this profession tier and its trainer recipes?", ProfCost(profs[index].scale, tier) * GOLD, false);
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_PROF); break;
                }
                CloseGossipMenuFor(player); return true;
        }
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }
};
void AddJarvisScripts() { new npc_jarvis(); }
