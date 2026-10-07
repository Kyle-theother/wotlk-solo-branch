#include "Chat.h"
#include "CreatureScript.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

constexpr uint32 GOLD_REP = 200000;
constexpr uint32 GOLD_PROF = 200000;
constexpr uint32 GOLD_GEAR = 10000;

enum JarvisAction
{
    ACT_ROOT = 1,
    ACT_CITIES = 2,
    ACT_RAIDS = 3,
    ACT_ZONES = 4,
    ACT_REP = 5,
    ACT_PROF = 6,
    ACT_GEAR = 7
};

struct JarvisDest
{
    char const* name;
    uint32 map;
    float x, y, z, o;
};

struct JarvisFaction
{
    char const* name;
    uint32 id;
};

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
    { "Obsidian Sanctum", 571, 3472.0f, 264.0f, -120.0f, 3.2f },
    { "Eye of Eternity", 571, 3860.0f, 6985.0f, 152.0f, 5.8f },
    { "Vault of Archavon", 571, 5440.0f, 2840.0f, 420.0f, 0.0f },
    { "Onyxia's Lair", 1, -4707.0f, -3726.0f, 54.0f, 3.4f },
    { "Molten Core", 0, -7523.0f, -1228.0f, 285.0f, 4.5f },
    { "Blackwing Lair", 0, -7672.0f, -1107.0f, 397.0f, 3.1f },
    { "Ahn'Qiraj", 1, -8242.0f, 1991.0f, 129.0f, 0.9f },
    { "Karazhan", 0, -11118.0f, -2011.0f, 47.0f, 0.6f },
    { "Serpentshrine Cavern", 530, 795.0f, 6867.0f, -65.0f, 6.2f },
    { "The Eye", 530, 3087.0f, 1383.0f, 185.0f, 4.6f },
    { "Black Temple", 530, -3644.0f, 316.0f, 35.0f, 2.9f },
    { "Sunwell Plateau", 530, 12574.0f, -6774.0f, 15.0f, 3.1f }
};

static JarvisDest const zones[] =
{
    { "Stranglethorn Vale", 0, -14302.0f, 518.0f, 8.0f, 2.6f },
    { "Silithus", 1, -6812.0f, 834.0f, 50.0f, 4.7f }
};

static JarvisFaction const factions[] =
{
    { "Stormwind", 72 },
    { "Ironforge", 47 },
    { "Darnassus", 69 },
    { "Exodar", 930 },
    { "Gnomeregan", 54 },
    { "Orgrimmar", 76 },
    { "Thunder Bluff", 81 },
    { "Undercity", 68 },
    { "Silvermoon", 911 },
    { "Darkspear Trolls", 530 },
    { "Cenarion Circle", 609 },
    { "Argent Crusade", 1106 },
    { "Kirin Tor", 1090 },
    { "Knights of the Ebon Blade", 1098 },
    { "Wyrmrest Accord", 1091 }
};

static uint32 const professions[] = { 164, 165, 171, 182, 186, 197, 202, 333, 393, 755, 773, 129, 185, 356 };
static uint8 const gearLevels[] = { 1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80 };

static bool TakeGold(Player* player, uint32 copper, char const* what)
{
    if (player->GetMoney() < copper)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Jarvis requires {} gold for {}.", copper / 10000, what);
        return false;
    }
    player->ModifyMoney(-int32(copper));
    return true;
}

static void GiveGear(Player* player, uint8 level, uint8 role)
{
    uint32 armorSub = 1;
    switch (player->getClass())
    {
        case CLASS_WARRIOR:
        case CLASS_PALADIN:
        case CLASS_DEATH_KNIGHT: armorSub = 4; break;
        case CLASS_HUNTER:
        case CLASS_SHAMAN: armorSub = 3; break;
        case CLASS_ROGUE:
        case CLASS_DRUID: armorSub = 2; break;
        default: armorSub = 1; break;
    }
    uint32 stat = role == 1 ? 7 : (role == 4 ? 3 : 5);
    uint32 minLevel = level > 5 ? level - 5 : 1;
    QueryResult result = WorldDatabase.Query(
        "SELECT entry, InventoryType FROM item_template "
        "WHERE Quality = 2 AND class = 4 AND subclass = {} AND RequiredLevel BETWEEN {} AND {} "
        "AND (AllowableClass = -1 OR AllowableClass & {}) AND InventoryType IN (1,3,5,6,7,8,9,10) "
        "AND (stat_type1 = {} OR stat_type2 = {} OR stat_type3 = {}) "
        "ORDER BY RequiredLevel DESC",
        armorSub, minLevel, level, player->getClassMask(), stat, stat, stat);
    if (!result)
    {
        ChatHandler(player->GetSession()).SendSysMessage("Jarvis has no green set for that level and role.");
        return;
    }
    bool used[32] = {};
    uint32 given = 0;
    do
    {
        uint32 entry = (*result)[0].Get<uint32>();
        uint32 slot = (*result)[1].Get<uint32>();
        if (slot < 32 && !used[slot])
        {
            used[slot] = true;
            if (player->AddItem(entry, 1))
                ++given;
        }
    } while (result->NextRow());
    ChatHandler(player->GetSession()).PSendSysMessage("Jarvis handed over {} green pieces.", given);
}

class npc_jarvis : public CreatureScript
{
public:
    npc_jarvis() : CreatureScript("npc_jarvis") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Cities", GOSSIP_SENDER_MAIN, ACT_CITIES);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Raids", GOSSIP_SENDER_MAIN, ACT_RAIDS);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Stranglethorn and Silithus", GOSSIP_SENDER_MAIN, ACT_ZONES);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Buy exalted reputation (2000 gold)", GOSSIP_SENDER_MAIN, ACT_REP);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Buy max professions (2000 gold)", GOSSIP_SENDER_MAIN, ACT_PROF);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Buy a green gear set (1 gold)", GOSSIP_SENDER_MAIN, ACT_GEAR);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        if (sender == ACT_CITIES && action < sizeof(cities) / sizeof(cities[0]))
        {
            JarvisDest const& d = cities[action];
            player->TeleportTo(d.map, d.x, d.y, d.z, d.o);
            CloseGossipMenuFor(player);
            return true;
        }
        if (sender == ACT_RAIDS && action < sizeof(raids) / sizeof(raids[0]))
        {
            JarvisDest const& d = raids[action];
            player->TeleportTo(d.map, d.x, d.y, d.z, d.o);
            CloseGossipMenuFor(player);
            return true;
        }
        if (sender == ACT_ZONES && action < sizeof(zones) / sizeof(zones[0]))
        {
            JarvisDest const& d = zones[action];
            player->TeleportTo(d.map, d.x, d.y, d.z, d.o);
            CloseGossipMenuFor(player);
            return true;
        }
        if (sender == ACT_REP && action < sizeof(factions) / sizeof(factions[0]))
        {
            if (TakeGold(player, GOLD_REP, "that reputation"))
            {
                if (FactionEntry const* faction = sFactionStore.LookupEntry(factions[action].id))
                    player->GetReputationMgr().SetOneFactionReputation(faction, 42000, false);
            }
            CloseGossipMenuFor(player);
            return true;
        }
        if (sender == ACT_GEAR && action >= 10)
        {
            uint8 level = action / 10;
            uint8 role = action % 10;
            if (TakeGold(player, GOLD_GEAR, "a gear set"))
                GiveGear(player, level, role);
            CloseGossipMenuFor(player);
            return true;
        }

        switch (action)
        {
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
            case ACT_REP:
                for (uint32 i = 0; i < sizeof(factions) / sizeof(factions[0]); ++i)
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, factions[i].name, ACT_REP, i);
                break;
            case ACT_PROF:
                if (TakeGold(player, GOLD_PROF, "max professions"))
                    for (uint32 skill : professions)
                        player->SetSkill(skill, 1, 450, 450);
                CloseGossipMenuFor(player);
                return true;
            case ACT_GEAR:
                for (uint8 level : gearLevels)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string("Level ") + std::to_string(level) + " tank", ACT_GEAR, level * 10 + 1);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string("Level ") + std::to_string(level) + " healer", ACT_GEAR, level * 10 + 2);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string("Level ") + std::to_string(level) + " caster", ACT_GEAR, level * 10 + 3);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string("Level ") + std::to_string(level) + " physical", ACT_GEAR, level * 10 + 4);
                }
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
