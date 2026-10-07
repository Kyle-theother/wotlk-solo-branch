#include "Chat.h"
#include "CreatureScript.h"
#include "DatabaseEnv.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include <string>

enum JarvisAction
{
    ACT_SALE = 1, ACT_PROF = 2, ACT_REP = 3, ACT_RESET = 4, ACT_HEIRLOOM = 5,
    ACT_TRAVEL = 6, ACT_CITIES = 7, ACT_RAIDS = 8, ACT_ZONES = 9,
    ACT_RACIAL = 20, ACT_TALENT = 21, ACT_SPELLS = 22, ACT_GEAR = 23, ACT_BACK = 99
};

static uint8 const gearLevels[] = { 1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80 };

static uint32 GearCost(uint8 level) { return level <= 1 ? 1 : uint32(level) * 100; }
static std::string GearPrice(uint8 level)
{
    if (level <= 1) return "1 copper";
    return std::to_string(level) + " silver";
}
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
static void GiveLevelGreens(Player* player, uint8 level)
{
    uint8 armor = ArmorFor(player, level);
    uint8 floor = level <= 5 ? 0 : level - 5;
    QueryResult armorRows = WorldDatabase.Query("SELECT entry FROM item_template WHERE Quality = 2 AND class = 4 AND subclass = {} AND InventoryType IN (1,3,5,6,7,8,9,10) AND RequiredLevel BETWEEN {} AND {} AND (AllowableClass = -1 OR AllowableClass & {}) GROUP BY InventoryType ORDER BY RequiredLevel DESC LIMIT 8", armor, floor, level, player->getClassMask());
    uint32 given = 0;
    if (armorRows)
    {
        do { player->AddItem(armorRows->Fetch()[0].Get<uint32>(), 1); ++given; } while (armorRows->NextRow());
    }
    QueryResult weapon = WorldDatabase.Query("SELECT entry FROM item_template WHERE Quality = 2 AND class = 2 AND RequiredLevel BETWEEN {} AND {} AND (AllowableClass = -1 OR AllowableClass & {}) ORDER BY RequiredLevel DESC LIMIT 1", floor, level, player->getClassMask());
    if (weapon) { player->AddItem(weapon->Fetch()[0].Get<uint32>(), 1); ++given; }
    if (!given) ChatHandler(player->GetSession()).SendSysMessage("No green gear was found for that level.");
}
