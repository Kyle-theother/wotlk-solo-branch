#include "Chat.h"
#include "ChatCommand.h"
#include "Creature.h"
#include "Formulas.h"
#include "Map.h"
#include "Player.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "World.h"
#include <cstdlib>

using namespace Acore::ChatCommands;

static constexpr char const* XP_SOURCE = "jarvis_xp";

static uint32 GetXpPercent(Player* player)
{
    if (!player->GetPlayerSetting(XP_SOURCE, 1).value)
        return 100;
    return player->GetPlayerSetting(XP_SOURCE, 0).value;
}

static void SetXpPercent(Player* player, uint32 percent)
{
    player->UpdatePlayerSetting(XP_SOURCE, 0, percent);
    player->UpdatePlayerSetting(XP_SOURCE, 1, 1);
}

class jarvis_xp_rate : public PlayerScript
{
public:
    jarvis_xp_rate() : PlayerScript("jarvis_xp_rate") { }

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* /*victim*/, uint8 /*xpSource*/) override
    {
        amount = amount * GetXpPercent(player) / 100;
    }

    void OnPlayerCreatureKill(Player* killer, Creature* killed) override
    {
        AwardRaidXp(killer, killed);
    }

    void OnPlayerCreatureKilledByPet(Player* petOwner, Creature* killed) override
    {
        AwardRaidXp(petOwner, killed);
    }

    static void AwardRaidXp(Player* player, Creature* killed)
    {
        if (!player || !killed || !player->GetMap() || !player->GetMap()->IsRaid())
            return;
        if (killed->IsTotem() || killed->IsPet() || killed->IsCritter())
            return;

        CreatureTemplate const* info = killed->GetCreatureTemplate();
        bool blocked = killed->HasFlagsExtra(CREATURE_FLAG_EXTRA_NO_XP) || !info || info->ModExperience <= 0.0f;
        if (!blocked)
            return;

        uint32 gain = Acore::XP::BaseGain(player->GetLevel(), killed->GetLevel(), GetContentLevelsForMapAndZone(killed->GetMapId(), killed->GetZoneId()));
        if (!gain)
            return;

        float xpMod = sWorld->getRate(RATE_XP_KILL);
        if (killed->isElite())
            xpMod *= 2.0f;
        gain = uint32(gain * xpMod);
        if (!gain)
            return;

        bool hadRecipient = killed->hasLootRecipient();
        if (!hadRecipient)
            killed->SetLootRecipient(player);
        player->GiveXP(gain, killed);
        if (!hadRecipient)
            killed->SetLootRecipient(nullptr);
    }
};

class jarvis_xp_commands : public CommandScript
{
public:
    jarvis_xp_commands() : CommandScript("jarvis_xp_commands") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable table =
        {
            { "xp", HandleXp, SEC_PLAYER, Console::No }
        };
        return table;
    }

    static bool HandleXp(ChatHandler* handler, Optional<std::string> value)
    {
        Player* player = handler->GetPlayer();
        if (!player)
            return false;

        if (!value)
        {
            handler->PSendSysMessage("XP rate is {}%. Use .xp 0 to 5. 1 is 100%, 5 is 500%.", GetXpPercent(player));
            return true;
        }

        char* end = nullptr;
        float rate = std::strtof(value->c_str(), &end);
        if (end == value->c_str() || rate < 0.0f || rate > 5.0f)
        {
            handler->SendSysMessage("Use .xp 0 to 5. 1 is 100%, 2.5 is 250%, 5 is 500%.");
            return false;
        }

        uint32 percent = uint32(rate * 100.0f + 0.5f);
        SetXpPercent(player, percent);
        handler->PSendSysMessage("XP rate set to {}%.", percent);
        return true;
    }
};

void AddJarvisXpScripts()
{
    new jarvis_xp_rate();
    new jarvis_xp_commands();
}
