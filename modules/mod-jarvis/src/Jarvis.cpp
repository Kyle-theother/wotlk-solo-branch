#include "Chat.h"
#include "CreatureScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include <string>

constexpr uint32 GOLD = 10000;
constexpr uint32 GOLD_ONE = 10000;

enum JarvisAction
{
    ACT_SALE = 1,
    ACT_PROF = 2,
    ACT_REP = 3,
    ACT_RESET = 4,
    ACT_HEIRLOOM = 5,
    ACT_TRAVEL = 6,
    ACT_CITIES = 7,
    ACT_RAIDS = 8,
    ACT_ZONES = 9,
    ACT_BACK = 99
};

struct JarvisDest { char const* name; uint32 map; float x, y, z, o; };
struct JarvisFaction { char const* name; uint32 id; uint8 team; uint32 gold; };
struct JarvisProf { char const* name; uint32 skill; uint16 scale; uint32 tool; uint32 spells[6]; };

static JarvisDest const cities[] =
{
    { "Stormwind", 0, -8832.0f, 628.0f, 94.0f, 0.7f },
    { "Ironforge", 0, -4920.0f, -946.0f, 502.0f, 5.4f },
    { "Darnassus", 1, 9866.0f, 2494.0f, 1316.0f, 5.5f },
    { "The Exodar", 530, -3965.0f, -11653.0f, -138.0f, 5.6f },
    { "Orgrimmar", 1, 1569.0f, -4420.0f, 16.0f, 0.0f },
    { "Thunder Bluff", 1, -1277.0f, 124.0f, 131.0f, 4.6f },
    { "Undercity", 0, 1632.0f, 240.0f, -43.0f, 6.2f },
    { "Silvermoon", 530, 9470.0f, -7278.0f, 14.0f, 6.1f },
    { "Shattrath", 530, -1833.0f, 5300.0f, -12.0f, 2.0f },
    { "Dalaran", 571, 5804.0f, 556.0f, 651.0f, 1.5f }
};
static JarvisDest const raids[] =
{
    { "Naxxramas", 571, 3668.0f, -1262.0f, 243.0f, 5.0f },
    { "Ulduar", 571, 9345.0f, -1114.0f, 1245.0f, 5.7f },
    { "Icecrown Citadel", 571, 5873.0f, 2110.0f, 636.0f, 1.5f },
    { "Onyxia's Lair", 1, -4707.0f, -3726.0f, 54.0f, 3.4f },
    { "Molten Core", 0, -7523.0f, -1228.0f, 285.0f, 4.5f },
    { "Karazhan", 0, -11118.0f, -2011.0f, 47.0f, 0.6f },
    { "Black Temple", 530, -3644.0f, 316.0f, 35.0f, 2.9f }
};
static JarvisDest const zones[] =
{
    { "Stranglethorn Vale", 0, -14302.0f, 518.0f, 8.0f, 2.6f },
    { "Silithus", 1, -6812.0f, 834.0f, 50.0f, 4.7f }
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

static uint32 ProfCost(uint16 scale, uint8 tier)
{
    uint32 base = tier == 1 ? 50 : (tier == 2 ? 750 : 1500);
    return base * scale / 100;
}
static uint16 ProfMax(uint8 tier) { return tier == 1 ? 300 : (tier == 2 ? 375 : 450); }
static char const* TierName(uint8 tier) { return tier == 1 ? "Vanilla" : (tier == 2 ? "TBC" : "Wrath"); }

static bool TakeGold(Player* player, uint32 gold)
{
    if (player->GetMoney() < gold * GOLD)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You do not have enough gold.");
        return false;
    }
    player->ModifyMoney(-int32(gold * GOLD));
    return true;
}

static void TeachProfession(Player* player, uint32 index, uint8 tier)
{
    JarvisProf const& prof = profs[index];
    uint32 count = tier == 1 ? 4 : (tier == 2 ? 5 : 6);
    for (uint32 i = 0; i < count; ++i)
        player->learnSpell(prof.spells[i], false);
    player->SetSkill(prof.skill, 1, ProfMax(tier), ProfMax(tier));
    if (prof.tool)
        player->AddItem(prof.tool, 1);
    if (prof.skill == 333)
        player->AddItem(tier == 1 ? 6218 : (tier == 2 ? 22463 : 44452), 1);
}

class npc_jarvis : public CreatureScript
{
public:
    npc_jarvis() : CreatureScript("npc_jarvis") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "What do you have for sale?", GOSSIP_SENDER_MAIN, ACT_SALE);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Learn professions", GOSSIP_SENDER_MAIN, ACT_PROF);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Can you help me gain reputation?", GOSSIP_SENDER_MAIN, ACT_REP);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "I'd like to reset all instances", GOSSIP_SENDER_MAIN, ACT_RESET, "Reset all instance locks for 1 gold?", GOLD_ONE, false);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "I'd like to purchase a class heirloom set", GOSSIP_SENDER_MAIN, ACT_HEIRLOOM);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Where can you take me?", GOSSIP_SENDER_MAIN, ACT_TRAVEL);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Nevermind", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (sender == ACT_CITIES && action < sizeof(cities) / sizeof(cities[0]))
        {
            CloseGossipMenuFor(player);
            player->TeleportTo(cities[action].map, cities[action].x, cities[action].y, cities[action].z, cities[action].o);
            return true;
        }
        if (sender == ACT_RAIDS && action < sizeof(raids) / sizeof(raids[0]))
        {
            CloseGossipMenuFor(player);
            player->TeleportTo(raids[action].map, raids[action].x, raids[action].y, raids[action].z, raids[action].o);
            return true;
        }
        if (sender == ACT_ZONES && action < sizeof(zones) / sizeof(zones[0]))
        {
            CloseGossipMenuFor(player);
            player->TeleportTo(zones[action].map, zones[action].x, zones[action].y, zones[action].z, zones[action].o);
            return true;
        }
        if (sender == ACT_REP)
        {
            uint32 seen = 0;
            uint8 team = player->GetTeamId() == TEAM_ALLIANCE ? 1 : 2;
            for (JarvisFaction const& faction : factions)
            {
                if (faction.team != 0 && faction.team != team)
                    continue;
                if (seen == action)
                {
                    CloseGossipMenuFor(player);
                    if (TakeGold(player, faction.gold))
                        if (FactionEntry const* entry = sFactionStore.LookupEntry(faction.id))
                            player->GetReputationMgr().SetOneFactionReputation(entry, 42000, false);
                    return true;
                }
                ++seen;
            }
        }
        if (sender == ACT_PROF && action >= 10)
        {
            uint32 index = action / 10 - 1;
            uint8 tier = action % 10;
            if (index < sizeof(profs) / sizeof(profs[0]) && tier >= 1 && tier <= 3)
            {
                CloseGossipMenuFor(player);
                if (TakeGold(player, ProfCost(profs[index].scale, tier)))
                    TeachProfession(player, index, tier);
                return true;
            }
        }
        if (sender == ACT_HEIRLOOM)
        {
            uint32 items[6] = {};
            uint32 count = 0;
            auto add = [&](std::initializer_list<uint32> list)
            {
                for (uint32 item : list)
                    if (count < 6)
                        items[count++] = item;
            };
            if (action == 11) add({ 42949, 48685, 42945, 48716, 42991 });
            else if (action == 12) add({ 42949, 48685, 42943, 44092, 42991 });
            else if (action == 21) add({ 42949, 48685, 42945, 44094, 42992 });
            else if (action == 22) add({ 42949, 48685, 44092, 48718, 42991 });
            else if (action == 31) add({ 42950, 48677, 42946, 44093, 42991 });
            else if (action == 41) add({ 42952, 48689, 42944, 44091, 42991 });
            else if (action == 51) add({ 42985, 48691, 42947, 42948, 42992 });
            else if (action == 61) add({ 42949, 48685, 42943, 42945, 42991 });
            else if (action == 71) add({ 42950, 48677, 42948, 44094, 42992 });
            else if (action == 81) add({ 42985, 48691, 42947, 44095, 42992 });
            else if (action == 91) add({ 42985, 48691, 42947, 44095, 42992 });
            else if (action == 111) add({ 42952, 48689, 42947, 48718, 42992 });
            if (count)
            {
                CloseGossipMenuFor(player);
                if (TakeGold(player, 1))
                    for (uint32 i = 0; i < count; ++i)
                        player->AddItem(items[i], 1);
                return true;
            }
        }

        ClearGossipMenuFor(player);
        if (action == ACT_BACK)
            return OnGossipHello(player, creature);
        switch (action)
        {
            case ACT_SALE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                return true;
            case ACT_RESET:
                CloseGossipMenuFor(player);
                if (TakeGold(player, 1))
                {
                    Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_ALL, false);
                    Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_ALL, true);
                }
                return true;
            case ACT_PROF:
                for (uint32 i = 0; i < sizeof(profs) / sizeof(profs[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, profs[i].name, GOSSIP_SENDER_MAIN, 200 + i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK);
                break;
            case ACT_REP:
            {
                uint8 team = player->GetTeamId() == TEAM_ALLIANCE ? 1 : 2;
                uint32 shown = 0;
                for (JarvisFaction const& faction : factions)
                {
                    if (faction.team != 0 && faction.team != team)
                        continue;
                    std::string line = std::string(faction.name) + " - " + std::to_string(faction.gold) + "g";
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, line, ACT_REP, shown, "Buy exalted reputation?", faction.gold * GOLD, false);
                    ++shown;
                }
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK);
                break;
            }
            case ACT_HEIRLOOM:
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Warrior, one-hand", ACT_HEIRLOOM, 11, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Warrior, two-hand", ACT_HEIRLOOM, 12, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Paladin, one-hand", ACT_HEIRLOOM, 21, "Buy this set for 1 gold? No heirloom shield exists in Wrath.", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Paladin, two-hand", ACT_HEIRLOOM, 22, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Hunter", ACT_HEIRLOOM, 31, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Rogue", ACT_HEIRLOOM, 41, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Priest", ACT_HEIRLOOM, 51, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Death Knight", ACT_HEIRLOOM, 61, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Shaman", ACT_HEIRLOOM, 71, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Mage", ACT_HEIRLOOM, 81, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Warlock", ACT_HEIRLOOM, 91, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Druid", ACT_HEIRLOOM, 111, "Buy this set for 1 gold?", GOLD_ONE, false);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK);
                break;
            case ACT_TRAVEL:
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Cities", GOSSIP_SENDER_MAIN, ACT_CITIES);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Raids", GOSSIP_SENDER_MAIN, ACT_RAIDS);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Stranglethorn and Silithus", GOSSIP_SENDER_MAIN, ACT_ZONES);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_BACK);
                break;
            case ACT_CITIES:
                for (uint32 i = 0; i < sizeof(cities) / sizeof(cities[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, cities[i].name, ACT_CITIES, i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_TRAVEL);
                break;
            case ACT_RAIDS:
                for (uint32 i = 0; i < sizeof(raids) / sizeof(raids[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, raids[i].name, ACT_RAIDS, i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_TRAVEL);
                break;
            case ACT_ZONES:
                for (uint32 i = 0; i < sizeof(zones) / sizeof(zones[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, zones[i].name, ACT_ZONES, i);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_TRAVEL);
                break;
            default:
                if (action >= 200 && action < 200 + sizeof(profs) / sizeof(profs[0]))
                {
                    uint32 index = action - 200;
                    for (uint8 tier = 1; tier <= 3; ++tier)
                    {
                        std::string line = std::string(TierName(tier)) + " " + profs[index].name + " - " + std::to_string(ProfCost(profs[index].scale, tier)) + "g";
                        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, line, ACT_PROF, (index + 1) * 10 + tier, "Learn this profession tier?", ProfCost(profs[index].scale, tier) * GOLD, false);
                    }
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, ACT_PROF);
                    break;
                }
                CloseGossipMenuFor(player);
                return true;
        }
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }
};

void AddJarvisScripts()
{
    new npc_jarvis();
}
