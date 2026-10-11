#include "Chat.h"
#include "Config.h"
#include "Creature.h"
#include "DungeonQuestMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "QuestDef.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "WorldSession.h"

#include <algorithm>
#include <vector>

namespace
{
bool EnableAcceptAll = true;
constexpr uint32 QuestSender = 1;
constexpr uint32 PageSender = 2;
constexpr uint32 ControlSender = 3;
constexpr uint32 TurnInSender = 4;
constexpr uint32 ChoiceSender = 5;
constexpr uint32 AcceptAllAction = 1;
constexpr uint32 GoodbyeAction = 2;
constexpr uint32 TurnInAllAction = 3;
constexpr uint32 VendorAction = 4;
constexpr uint32 BankAction = 5;
constexpr uint32 TravelSender = 6;
constexpr uint32 DestSender = 7;
constexpr uint32 TravelAction = 6;

struct GuideDest { char const* name; uint32 map; float x, y, z, o; };
static GuideDest const cities[] = {
    { "Stormwind", 0, -8832.0f, 628.0f, 94.0f, 0.7f }, { "Ironforge", 0, -4920.0f, -946.0f, 502.0f, 5.4f },
    { "Darnassus", 1, 9866.0f, 2494.0f, 1316.0f, 5.5f }, { "The Exodar", 530, -3965.0f, -11653.0f, -138.0f, 5.6f },
    { "Orgrimmar", 1, 1569.0f, -4420.0f, 16.0f, 0.0f }, { "Thunder Bluff", 1, -1277.0f, 124.0f, 131.0f, 4.6f },
    { "Undercity", 0, 1632.0f, 240.0f, -43.0f, 6.2f }, { "Silvermoon", 530, 9470.0f, -7278.0f, 14.0f, 6.1f },
    { "Shattrath", 530, -1833.0f, 5300.0f, -12.0f, 2.0f }, { "Dalaran", 571, 5804.0f, 556.0f, 651.0f, 1.5f }
};
static GuideDest const vanillaRaids[] = {
    { "Molten Core", 230, 1126.0f, -459.0f, -102.0f, 3.5f }, { "Onyxia's Lair", 1, -4708.0f, -3726.0f, 54.5f, 3.4f },
    { "Blackwing Lair", 229, 152.0f, -474.0f, 116.0f, 0.0f }, { "Zul'Gurub", 0, -11916.0f, -1244.0f, 92.0f, 4.7f },
    { "Ruins of Ahn'Qiraj", 1, -8409.0f, 1498.0f, 28.0f, 2.5f }, { "Temple of Ahn'Qiraj", 1, -8242.0f, 1991.0f, 129.0f, 0.9f },
    { "Upper Blackrock Spire", 0, -7527.0f, -1226.0f, 285.0f, 4.5f }
};
static GuideDest const tbcRaids[] = {
    { "Karazhan", 0, -11118.0f, -2011.0f, 47.0f, 0.7f }, { "Gruul's Lair", 530, 3539.0f, 5086.0f, 3.0f, 5.7f },
    { "Magtheridon's Lair", 530, -312.0f, 3087.0f, -116.0f, 5.2f }, { "Serpentshrine Cavern", 530, 797.0f, 6862.0f, -65.0f, 6.1f },
    { "Tempest Keep", 530, 3088.0f, 1384.0f, 185.0f, 4.6f }, { "Battle for Mount Hyjal", 1, -8177.0f, -4178.0f, -167.0f, 0.8f },
    { "Black Temple", 530, -3649.0f, 317.0f, 35.0f, 2.9f }, { "Sunwell Plateau", 530, 12574.0f, -6774.0f, 15.0f, 3.1f }
};
static GuideDest const wotlkRaids[] = {
    { "Naxxramas", 571, 3668.0f, -1262.0f, 243.0f, 5.0f }, { "Obsidian Sanctum", 571, 3457.0f, 262.0f, -113.0f, 3.0f },
    { "Eye of Eternity", 571, 3860.0f, 6984.0f, 152.0f, 5.6f }, { "Ulduar", 571, 9345.0f, -1114.0f, 1245.0f, 5.7f },
    { "Trial of the Crusader", 571, 8515.0f, 716.0f, 558.0f, 1.6f }, { "Onyxia's Lair", 1, -4708.0f, -3726.0f, 54.5f, 3.4f },
    { "Icecrown Citadel", 571, 5873.0f, 2110.0f, 636.0f, 1.5f }, { "Ruby Sanctum", 571, 3599.0f, 199.0f, -113.0f, 5.3f },
    { "Vault of Archavon", 571, 5453.0f, 2840.0f, 421.0f, 0.0f }
};
static GuideDest const ekZones[] = {
    { "Elwynn Forest", 0, -9449.0f, 64.0f, 56.0f, 0.0f }, { "Westfall", 0, -10630.0f, 1037.0f, 33.0f, 3.0f },
    { "Redridge Mountains", 0, -9216.0f, -2204.0f, 66.0f, 2.4f }, { "Duskwood", 0, -10516.0f, -1156.0f, 28.0f, 2.6f },
    { "Stranglethorn Vale", 0, -14302.0f, 518.0f, 8.0f, 2.6f }, { "Swamp of Sorrows", 0, -10452.0f, -3258.0f, 21.0f, 1.6f },
    { "Blasted Lands", 0, -11186.0f, -3016.0f, 7.0f, 3.8f }, { "Burning Steppes", 0, -7976.0f, -2104.0f, 127.0f, 5.0f },
    { "Searing Gorge", 0, -6684.0f, -1192.0f, 240.0f, 0.5f }, { "Badlands", 0, -6764.0f, -3136.0f, 241.0f, 3.1f },
    { "Deadwind Pass", 0, -10438.0f, -1876.0f, 105.0f, 4.2f }, { "Western Plaguelands", 0, 1726.0f, -1638.0f, 60.0f, 1.3f },
    { "Eastern Plaguelands", 0, 2280.0f, -5310.0f, 87.0f, 1.0f }, { "Tirisfal Glades", 0, 2260.0f, 289.0f, 34.0f, 2.4f },
    { "Silverpine Forest", 0, 505.0f, 1504.0f, 125.0f, 1.6f }, { "Hillsbrad Foothills", 0, -852.0f, -535.0f, 10.0f, 2.0f },
    { "Alterac Mountains", 0, 370.0f, -211.0f, 145.0f, 5.5f }, { "Arathi Highlands", 0, -1508.0f, -2734.0f, 32.0f, 3.1f },
    { "The Hinterlands", 0, 112.0f, -3920.0f, 136.0f, 2.2f }, { "Wetlands", 0, -3242.0f, -2464.0f, 15.0f, 0.8f },
    { "Loch Modan", 0, -5240.0f, -2930.0f, 336.0f, 5.2f }, { "Dun Morogh", 0, -5600.0f, -482.0f, 397.0f, 3.8f },
    { "Isle of Quel'Danas", 530, 12806.0f, -6911.0f, 6.0f, 2.4f }
};
static GuideDest const kalZones[] = {
    { "Teldrassil", 1, 10111.0f, 1557.0f, 1326.0f, 4.1f }, { "Darkshore", 1, 6348.0f, 561.0f, 16.0f, 1.8f },
    { "Ashenvale", 1, 2324.0f, -1666.0f, 132.0f, 1.2f }, { "Azshara", 1, 3341.0f, -4603.0f, 93.0f, 5.0f },
    { "Felwood", 1, 4102.0f, -1316.0f, 220.0f, 4.6f }, { "Winterspring", 1, 6815.0f, -4610.0f, 710.0f, 4.3f },
    { "Moonglade", 1, 7992.0f, -2679.0f, 512.0f, 3.1f }, { "Stonetalon Mountains", 1, 1574.0f, 1031.0f, 137.0f, 3.3f },
    { "Desolace", 1, -1596.0f, 3140.0f, 50.0f, 5.6f }, { "Feralas", 1, -4419.0f, 237.0f, 26.0f, 3.1f },
    { "Thousand Needles", 1, -4904.0f, -1344.0f, -49.0f, 2.2f }, { "Dustwallow Marsh", 1, -3464.0f, -4124.0f, 17.0f, 3.4f },
    { "Tanaris", 1, -7178.0f, -3786.0f, 9.0f, 2.8f }, { "Un'Goro Crater", 1, -7944.0f, -2118.0f, -218.0f, 5.8f },
    { "Silithus", 1, -6812.0f, 834.0f, 50.0f, 4.7f }, { "Durotar", 1, 1359.0f, -4369.0f, 26.0f, 0.3f },
    { "The Barrens", 1, -456.0f, -2648.0f, 96.0f, 4.0f }, { "Mulgore", 1, -2324.0f, -392.0f, -8.0f, 2.7f },
    { "Azuremyst Isle", 530, -4203.0f, -12512.0f, 44.0f, 1.6f }, { "Bloodmyst Isle", 530, -2095.0f, -11857.0f, 49.0f, 1.2f }
};
static GuideDest const outZones[] = {
    { "Hellfire Peninsula", 530, -248.0f, 966.0f, 84.0f, 1.6f }, { "Zangarmarsh", 530, -204.0f, 5498.0f, 22.0f, 1.5f },
    { "Terokkar Forest", 530, -1944.0f, 4688.0f, 3.0f, 3.1f }, { "Nagrand", 530, -1516.0f, 7448.0f, -8.0f, 0.6f },
    { "Blade's Edge Mountains", 530, 2024.0f, 6604.0f, 134.0f, 1.2f }, { "Netherstorm", 530, 4150.0f, 3016.0f, 339.0f, 4.4f },
    { "Shadowmoon Valley", 530, -3184.0f, 2704.0f, 94.0f, 2.6f }
};
static GuideDest const nrZones[] = {
    { "Borean Tundra", 571, 2926.0f, 6176.0f, 99.0f, 3.6f }, { "Howling Fjord", 571, 1340.0f, -4936.0f, 174.0f, 0.9f },
    { "Dragonblight", 571, 3712.0f, 228.0f, 48.0f, 3.1f }, { "Grizzly Hills", 571, 3376.0f, -2128.0f, 121.0f, 1.4f },
    { "Zul'Drak", 571, 5448.0f, -2592.0f, 292.0f, 0.6f }, { "Sholazar Basin", 571, 5568.0f, 5752.0f, -75.0f, 1.7f },
    { "The Storm Peaks", 571, 7856.0f, -816.0f, 1178.0f, 5.6f }, { "Icecrown", 571, 7256.0f, 2040.0f, 530.0f, 1.2f }
};
static GuideDest const* TravelList(uint32 category, uint32& count)
{
    switch (category)
    {
        case 1: count = sizeof(cities) / sizeof(cities[0]); return cities;
        case 2: count = sizeof(vanillaRaids) / sizeof(vanillaRaids[0]); return vanillaRaids;
        case 3: count = sizeof(tbcRaids) / sizeof(tbcRaids[0]); return tbcRaids;
        case 4: count = sizeof(wotlkRaids) / sizeof(wotlkRaids[0]); return wotlkRaids;
        case 5: count = sizeof(ekZones) / sizeof(ekZones[0]); return ekZones;
        case 6: count = sizeof(kalZones) / sizeof(kalZones[0]); return kalZones;
        case 7: count = sizeof(outZones) / sizeof(outZones[0]); return outZones;
        case 8: count = sizeof(nrZones) / sizeof(nrZones[0]); return nrZones;
        default: count = 0; return nullptr;
    }
}
void ShowTravel(Player* player, Creature* creature, uint32 category)
{
    ClearGossipMenuFor(player);
    if (category == 0)
    {
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Cities", TravelSender, 1);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Vanilla raids", TravelSender, 2);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "The Burning Crusade raids", TravelSender, 3);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Wrath of the Lich King raids", TravelSender, 4);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Eastern Kingdoms", TravelSender, 5);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Kalimdor", TravelSender, 6);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Outland", TravelSender, 7);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Northrend", TravelSender, 8);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", PageSender, 0);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return;
    }
    uint32 count = 0;
    GuideDest const* list = TravelList(category, count);
    for (uint32 i = 0; i < count; ++i)
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, list[i].name, DestSender, (category << 8) | i);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", TravelSender, 0);
    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
}
// Leave room for previous/next, accept-all and goodbye below the core's 32-item limit.
constexpr uint32 PageSize = 24;

bool IsGuideAvailable(Player* player, Creature* creature)
{
    return sDungeonQuestMgr->IsEnabled() && player->IsAlive()
        && player->GetMap() == creature->GetMap()
        && sDungeonQuestMgr->IsDungeonMap(player->GetMapId());
}

Quest const* GetEligibleQuest(Player* player, uint32 questId)
{
    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    // IsQuestRewarded also covers previously rewarded repeatables, as requested.
    // GetQuestRewardStatus alone deliberately does not cover those in the core.
    if (!quest || player->GetQuestStatus(questId) != QUEST_STATUS_NONE || player->IsQuestRewarded(questId))
        return nullptr;

    // Core authority for disabled quests, chains, conditions, class/race, level,
    // reputation, exclusive groups, timers, quest-log and source-item capacity.
    return player->CanTakeQuest(quest, false) && player->CanAddQuest(quest, false) ? quest : nullptr;
}

std::vector<uint32> GetEligibleQuests(Player* player)
{
    std::vector<uint32> result;
    for (uint32 questId : sDungeonQuestMgr->GetDungeonQuests(player->GetMapId()))
        if (GetEligibleQuest(player, questId))
            result.push_back(questId);
    return result;
}

bool AcceptQuest(Player* player, Creature* creature, uint32 questId)
{
    if (!IsGuideAvailable(player, creature))
        return false;

    // Recheck current-map membership too: never trust a stale client selection.
    auto const& quests = sDungeonQuestMgr->GetDungeonQuests(player->GetMapId());
    if (std::find(quests.begin(), quests.end(), questId) == quests.end())
        return false;

    Quest const* quest = GetEligibleQuest(player, questId);
    if (!quest)
        return false;

    player->AddQuestAndCheckCompletion(quest, creature);
    QuestStatus status = player->GetQuestStatus(questId);
    return status == QUEST_STATUS_INCOMPLETE || status == QUEST_STATUS_COMPLETE;
}


bool IsDungeonQuest(Player* player, uint32 questId)
{
    auto const& quests = sDungeonQuestMgr->GetDungeonQuests(player->GetMapId());
    return std::find(quests.begin(), quests.end(), questId) != quests.end();
}

Quest const* GetTurnInQuest(Player* player, uint32 questId)
{
    if (!IsDungeonQuest(player, questId))
        return nullptr;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (!quest || player->GetQuestStatus(questId) != QUEST_STATUS_COMPLETE)
        return nullptr;

    return player->CanRewardQuest(quest, false) ? quest : nullptr;
}

std::vector<uint32> GetTurnInQuests(Player* player)
{
    std::vector<uint32> result;
    for (uint32 questId : sDungeonQuestMgr->GetDungeonQuests(player->GetMapId()))
        if (GetTurnInQuest(player, questId))
            result.push_back(questId);
    return result;
}

bool TurnInQuest(Player* player, Creature* creature, uint32 questId, uint32 choice)
{
    if (!IsGuideAvailable(player, creature))
        return false;

    Quest const* quest = GetTurnInQuest(player, questId);
    if (!quest || !player->CanRewardQuest(quest, choice, false))
        return false;

    player->RewardQuest(quest, choice, creature);
    return player->GetQuestStatus(questId) != QUEST_STATUS_COMPLETE;
}

void ShowMenu(Player* player, Creature* creature, uint32 page = 0)
{
    ClearGossipMenuFor(player);
    bool hasQuests = IsGuideAvailable(player, creature)
        && !sDungeonQuestMgr->GetDungeonQuests(player->GetMapId()).empty();

    std::vector<uint32> turnIns = hasQuests ? GetTurnInQuests(player) : std::vector<uint32>();
    std::vector<uint32> quests = hasQuests ? GetEligibleQuests(player) : std::vector<uint32>();

    for (uint32 questId : turnIns)
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Turn in: " + sDungeonQuestMgr->GetQuestTitle(questId),
            TurnInSender, questId);

    uint32 lastPage = quests.empty() ? 0 : static_cast<uint32>((quests.size() - 1) / PageSize);
    page = std::min(page, lastPage);
    std::size_t begin = static_cast<std::size_t>(page) * PageSize;
    std::size_t end = quests.empty() ? 0 : std::min(begin + PageSize, quests.size());
    for (std::size_t index = begin; index < end; ++index)
        AddGossipItemFor(player, GOSSIP_ICON_DOT, sDungeonQuestMgr->GetQuestTitle(quests[index]),
            QuestSender, quests[index]);

    if (page > 0)
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Previous page", PageSender, page - 1);
    if (page < lastPage)
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Next page", PageSender, page + 1);
    if (EnableAcceptAll && !quests.empty())
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Accept All Available Quests", ControlSender, AcceptAllAction);
    if (turnIns.size() > 1)
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Turn In All Completed Quests", ControlSender, TurnInAllAction);
    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "I want to browse your goods.", ControlSender, VendorAction);
    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "I want to access my bank.", ControlSender, BankAction);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Teleport services.", ControlSender, TravelAction);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye", ControlSender, GoodbyeAction);
    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
}

class npc_dungeon_quest_guide : public CreatureScript
{
public:
    npc_dungeon_quest_guide() : CreatureScript("npc_dungeon_quest_guide") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ShowMenu(player, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        if (!IsGuideAvailable(player, creature) || (sender == ControlSender && action == GoodbyeAction))
        {
            CloseGossipMenuFor(player);
            return true;
        }

        if (sender == PageSender)
        {
            ShowMenu(player, creature, action);
            return true;
        }

        if (sender == ControlSender && action == VendorAction)
        {
            CloseGossipMenuFor(player);
            player->GetSession()->SendListInventory(creature->GetGUID());
            return true;
        }

        if ((sender == ControlSender && action == TravelAction) || sender == TravelSender)
        {
            ShowTravel(player, creature, sender == TravelSender ? action : 0);
            return true;
        }
        if (sender == DestSender)
        {
            uint32 category = action >> 8;
            uint32 index = action & 0xFF;
            uint32 count = 0;
            GuideDest const* list = TravelList(category, count);
            CloseGossipMenuFor(player);
            if (list && index < count)
                player->TeleportTo(list[index].map, list[index].x, list[index].y, list[index].z, list[index].o);
            return true;
        }
        if (sender == ControlSender && action == BankAction)
        {
            CloseGossipMenuFor(player);
            player->GetSession()->SendShowBank(creature->GetGUID());
            return true;
        }

        if (sender == QuestSender)
        {
            if (!AcceptQuest(player, creature, action))
                ChatHandler(player->GetSession()).SendSysMessage("This quest could not be accepted.");
        }
        else if (sender == TurnInSender)
        {
            Quest const* quest = GetTurnInQuest(player, action);
            if (!quest)
                ChatHandler(player->GetSession()).SendSysMessage("This quest could not be turned in.");
            else if (quest->GetRewChoiceItemsCount() > 1)
            {
                ClearGossipMenuFor(player);
                for (uint32 index = 0; index < quest->GetRewChoiceItemsCount(); ++index)
                {
                    ItemTemplate const* item = sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[index]);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, item ? item->Name1 : "Reward",
                        ChoiceSender, (action << 8) | index);
                }
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", PageSender, 0);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                return true;
            }
            else if (!TurnInQuest(player, creature, action, 0))
                ChatHandler(player->GetSession()).SendSysMessage("This quest could not be turned in.");
        }
        else if (sender == ChoiceSender)
        {
            uint32 questId = action >> 8;
            uint32 choice = action & 0xFF;
            if (!TurnInQuest(player, creature, questId, choice))
                ChatHandler(player->GetSession()).SendSysMessage("This quest could not be turned in.");
        }
        else if (sender == ControlSender && action == TurnInAllAction)
        {
            uint32 turnedIn = 0;
            for (uint32 questId : GetTurnInQuests(player))
            {
                Quest const* quest = GetTurnInQuest(player, questId);
                if (!quest || quest->GetRewChoiceItemsCount() > 1)
                    continue;
                if (TurnInQuest(player, creature, questId, 0))
                    ++turnedIn;
            }
            if (turnedIn)
                ChatHandler(player->GetSession()).PSendSysMessage("{} dungeon quests turned in.", turnedIn);
            else
                ChatHandler(player->GetSession()).SendSysMessage("Completed quests with a reward choice must be turned in individually.");
        }
        else if (sender == ControlSender && action == AcceptAllAction && EnableAcceptAll)
        {
            // Snapshot only currently eligible quests; never automatically traverse a chain.
            std::vector<uint32> quests = GetEligibleQuests(player);
            uint32 accepted = 0;
            for (uint32 questId : quests)
                if (AcceptQuest(player, creature, questId))
                    ++accepted;

            if (accepted)
                ChatHandler(player->GetSession()).PSendSysMessage("{} dungeon quests accepted.", accepted);
            else
                ChatHandler(player->GetSession()).SendSysMessage("No dungeon quests could be accepted.");
        }

        ShowMenu(player, creature);
        return true;
    }
};
}

void LoadDungeonQuestGuideConfig()
{
    EnableAcceptAll = sConfigMgr->GetOption<bool>("DungeonQuests.EnableAcceptAll", true);
}

void AddSC_npc_dungeon_quest_guide()
{
    new npc_dungeon_quest_guide();
}
