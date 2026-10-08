#include "Player.h"
#include "PlayerScript.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"

static uint32 const atWarFactions[] = { 67, 76, 81, 68, 530, 911, 469, 72, 47, 69, 54, 930 };

class at_war_factions : public PlayerScript
{
public:
    at_war_factions() : PlayerScript("at_war_factions") { }

    void OnPlayerLogin(Player* player) override
    {
        if (!player)
            return;
        for (uint32 id : atWarFactions)
        {
            FactionEntry const* entry = sFactionStore.LookupEntry(id);
            if (!entry)
                continue;
            if (FactionState* state = player->GetReputationMgr().GetState(entry))
            {
                state->Flags &= ~(FACTION_FLAG_PEACE_FORCED | FACTION_FLAG_INVISIBLE_FORCED | FACTION_FLAG_HIDDEN);
                state->Flags |= FACTION_FLAG_VISIBLE;
                player->GetReputationMgr().SendState(state);
            }
        }
    }
};

void AddJarvisAtWarScripts()
{
    new at_war_factions();
}
