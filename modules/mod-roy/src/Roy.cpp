#include "CreatureScript.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

enum RoyGossip
{
    GOSSIP_VENDOR = 1,
    GOSSIP_DALARAN = 2,
    GOSSIP_CAPITAL = 3
};

class roy : public CreatureScript
{
public:
    roy() : CreatureScript("roy") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Show me your goods.", GOSSIP_SENDER_MAIN, GOSSIP_VENDOR);
        if (player->GetLevel() >= 80)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Teleport to Dalaran.", GOSSIP_SENDER_MAIN, GOSSIP_DALARAN);
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Teleport to my capital.", GOSSIP_SENDER_MAIN, GOSSIP_CAPITAL);
        }
        else
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Come back at level 80 for teleports.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
            case GOSSIP_VENDOR:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
            case GOSSIP_DALARAN:
                if (player->GetLevel() >= 80)
                    player->TeleportTo(571, 5804.0f, 556.0f, 651.0f, 1.5f);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_CAPITAL:
                if (player->GetLevel() >= 80)
                {
                    if (player->GetTeamId() == TEAM_ALLIANCE)
                        player->TeleportTo(0, -8832.0f, 628.0f, 94.0f, 0.7f);
                    else
                        player->TeleportTo(1, 1569.0f, -4420.0f, 16.0f, 0.0f);
                }
                CloseGossipMenuFor(player);
                break;
            default:
                CloseGossipMenuFor(player);
                break;
        }
        return true;
    }
};

void Addmod_royScripts()
{
    new roy();
}
