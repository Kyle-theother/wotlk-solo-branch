#include "Chat.h"
#include "CreatureScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

constexpr uint32 GOLD_REP = 200000;
constexpr uint32 GOLD_PROF = 200000;
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
    ACT_ZONES = 9
};

struct JarvisDest { char const* name; uint32 map; float x, y, z, o; };
struct JarvisFaction { char const* name; uint32 id; };

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
    { "Stormwind", 72 }, { "Ironforge", 47 }, { "Darnassus", 69 }, { "Exodar", 930 }, { "Gnomeregan", 54 },
    { "Orgrimmar", 76 }, { "Thunder Bluff", 81 }, { "Undercity", 68 }, { "Silvermoon", 911 }, { "Darkspear Trolls", 530 },
    { "Cenarion Circle", 609 }, { "Argent Crusade", 1106 }, { "Kirin Tor", 1090 }, { "Knights of the Ebon Blade", 1098 }, { "Wyrmrest Accord", 1091 }
};

static uint32 const professionSpells[] =
{
    2259, 3101, 3464, 11611, 28596, 51304,
    2018, 3100, 3538, 9785, 29844, 51300,
    7411, 7412, 7413, 13920, 28029, 51313,
    4036, 4037, 4038, 12656, 30350, 51306,
    2366, 2368, 3570, 11993, 28695, 50300,
    45357, 45358, 45359, 45360, 45361, 45363,
    25229, 25230, 28894, 28895, 28897, 51311,
    2108, 3104, 3811, 10662, 32549, 51302,
    2575, 2576, 3564, 10248, 29354, 50310,
    8613, 8617, 8618, 10768, 32678, 50305,
    3908, 3909, 3910, 12180, 26790, 51309,
    2550, 3102, 3413, 18260, 33359, 51296,
    3273, 3274, 7924, 10846, 27028, 45542,
    7620, 7731, 7732, 18248, 33095, 51294
};

static uint32 const professionSkills[] = { 171, 164, 333, 202, 182, 773, 755, 165, 186, 393, 197, 185, 129, 356 };
static char const* const classNames[] = { "", "Warrior", "Paladin", "Hunter", "Rogue", "Priest", "Death Knight", "Shaman", "Mage", "Warlock", "", "Druid" };
static uint32 const heirloomSets[][6] =
{
    { 0 },
    { 42949, 48685, 42943, 42945, 42992, 0 },
    { 42949, 48685, 42945, 44094, 42992, 0 },
    { 42950, 48677, 42946, 44093, 42991, 0 },
    { 42952, 48689, 42944, 44091, 42991, 0 },
    { 42985, 48691, 42947, 42948, 42992, 0 },
    { 42949, 48685, 42943, 42945, 42992, 0 },
    { 42950, 48677, 42948, 44094, 42992, 0 },
    { 42985, 48691, 42947, 44095, 42992, 0 },
    { 42985, 48691, 42947, 44095, 42992, 0 },
    { 0 },
    { 42952, 48689, 42947, 48718, 42992, 0 }
};

static bool TakeGold(Player* player, uint32 copper)
{
    if (player->GetMoney() < copper)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You do not have enough gold.");
        return false;
    }
    player->ModifyMoney(-int32(copper));
    return true;
}

class npc_jarvis : public CreatureScript
{
public:
    npc_jarvis() : CreatureScript("npc_jarvis") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "What do you have for sale?", GOSSIP_SENDER_MAIN, ACT_SALE);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Which professions can you teach me?", GOSSIP_SENDER_MAIN, ACT_PROF, "Learn every profession at 450 for 2000 gold?", GOLD_PROF, false);
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
        if (sender == ACT_REP && action < sizeof(factions) / sizeof(factions[0]))
        {
            CloseGossipMenuFor(player);
            if (TakeGold(player, GOLD_REP))
                if (FactionEntry const* faction = sFactionStore.LookupEntry(factions[action].id))
                    player->GetReputationMgr().SetOneFactionReputation(faction, 42000, false);
            return true;
        }
        if (sender == ACT_HEIRLOOM && action >= CLASS_WARRIOR && action <= CLASS_DRUID && action != 10)
        {
            CloseGossipMenuFor(player);
            if (TakeGold(player, GOLD_ONE))
                for (uint32 item : heirloomSets[action])
                    if (item)
                        player->AddItem(item, 1);
            return true;
        }

        ClearGossipMenuFor(player);
        switch (action)
        {
            case ACT_SALE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                return true;
            case ACT_PROF:
                CloseGossipMenuFor(player);
                if (TakeGold(player, GOLD_PROF))
                {
                    for (uint32 spell : professionSpells)
                        player->learnSpell(spell, false);
                    for (uint32 skill : professionSkills)
                        player->SetSkill(skill, 1, 450, 450);
                }
                return true;
            case ACT_RESET:
                CloseGossipMenuFor(player);
                if (TakeGold(player, GOLD_ONE))
                {
                    Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_ALL, false);
                    Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_ALL, true);
                }
                return true;
            case ACT_REP:
                for (uint32 i = 0; i < sizeof(factions) / sizeof(factions[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, factions[i].name, ACT_REP, i, "Buy exalted reputation for 2000 gold?", GOLD_REP, false);
                break;
            case ACT_HEIRLOOM:
                for (uint8 classId = CLASS_WARRIOR; classId <= CLASS_DRUID; ++classId)
                    if (classId != 10)
                        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, classNames[classId], ACT_HEIRLOOM, classId, "Buy this heirloom set for 1 gold?", GOLD_ONE, false);
                break;
            case ACT_TRAVEL:
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Cities", GOSSIP_SENDER_MAIN, ACT_CITIES);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Raids", GOSSIP_SENDER_MAIN, ACT_RAIDS);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Stranglethorn and Silithus", GOSSIP_SENDER_MAIN, ACT_ZONES);
                break;
            case ACT_CITIES:
                for (uint32 i = 0; i < sizeof(cities) / sizeof(cities[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, cities[i].name, ACT_CITIES, i);
                break;
            case ACT_RAIDS:
                for (uint32 i = 0; i < sizeof(raids) / sizeof(raids[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, raids[i].name, ACT_RAIDS, i);
                break;
            case ACT_ZONES:
                for (uint32 i = 0; i < sizeof(zones) / sizeof(zones[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, zones[i].name, ACT_ZONES, i);
                break;
            default:
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
