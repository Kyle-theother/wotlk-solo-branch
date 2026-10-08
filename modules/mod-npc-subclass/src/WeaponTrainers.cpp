#include "Chat.h"
#include "DatabaseEnv.h"
#include "CreatureScript.h"
#include "Player.h"
#include "PlayerScript.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

struct WeaponSkill
{
    uint32 spellId;
    uint16 skillId;
    char const* name;
};

static WeaponSkill const weaponSkills[] =
{
    { 0, 44, "1H Axe" },
    { 0, 172, "2H Axe" },
    { 0, 43, "1H Sword" },
    { 0, 55, "2H Sword" },
    { 0, 54, "1H Mace" },
    { 0, 160, "2H Mace" },
    { 0, 229, "Polearm" },
    { 0, 173, "Dagger" },
    { 0, 473, "Fist" },
    { 0, 136, "Staff" },
    { 5019, 228, "Wand" },
    { 0, 176, "Thrown" },
    { 3018, 45, "Bow" },
    { 3018, 226, "Crossbow" },
    { 3018, 46, "Gun" },
    { 9077, 414, "Leather" },
    { 8737, 413, "Mail" },
    { 750, 293, "Plate" },
    { 9116, 433, "Shield" }
};

static uint32 SkillForSpell(uint32 spellId)
{
    switch (spellId)
    {
        case 196: return 44;
        case 197: return 172;
        case 201: return 43;
        case 202: return 55;
        case 198: return 54;
        case 199: return 160;
        case 200: return 229;
        case 1180: return 173;
        case 15590: return 473;
        case 227: return 136;
        case 5009: return 228;
        case 2567: return 176;
        case 264: return 45;
        case 5011: return 226;
        case 266: return 46;
        case 9077: return 414;
        case 8737: return 413;
        case 750: return 293;
        case 9116: return 433;
        default: return 0;
    }
}

static void GrantWeaponSkill(Player* player, WeaponSkill const& skill)
{
    if (skill.spellId && !player->HasSpell(skill.spellId))
        player->learnSpell(skill.spellId, false);
    if (player->GetSkillValue(skill.skillId) == 0)
        player->SetSkill(skill.skillId, 0, 1, 1);
}

class weapon_trainer_skills : public PlayerScript
{
public:
    weapon_trainer_skills() : PlayerScript("weapon_trainer_skills") { }

    void OnPlayerLearnSpell(Player* player, uint32 spellId) override
    {
        uint32 skillId = SkillForSpell(spellId);
        if (player && skillId && player->GetSkillValue(skillId) == 0)
            player->SetSkill(skillId, 0, 1, 1);
    }
};

class weapon_trainer_npc : public CreatureScript
{
public:
    weapon_trainer_npc() : CreatureScript("weapon_trainer_npc") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!creature->IsTrainer())
            return false;
        QueryResult result = WorldDatabase.Query("SELECT 1 FROM creature_default_trainer cdt JOIN trainer_spell ts ON ts.TrainerId = cdt.TrainerId WHERE cdt.CreatureId = {} AND ts.SpellId IN (196,197,198,199,200,201,202,227,264,266,1180,2567,5009,5011,15590) LIMIT 1", creature->GetEntry());
        if (!result)
            return false;
        ClearGossipMenuFor(player);
        for (uint32 i = 0; i < sizeof(weaponSkills) / sizeof(weaponSkills[0]); ++i)
        {
            if (player->GetSkillValue(weaponSkills[i].skillId) > 0)
                continue;
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, std::string(weaponSkills[i].name) + " - 1c", GOSSIP_SENDER_MAIN, 100 + i, "Learn this weapon skill for 1 copper?", 1, false);
        }
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Trainer", GOSSIP_SENDER_MAIN, 1);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (action == 1)
        {
            CloseGossipMenuFor(player);
            player->GetSession()->SendTrainerList(creature->GetGUID());
            return true;
        }
        if (action < 100 || action >= 100 + sizeof(weaponSkills) / sizeof(weaponSkills[0]))
            return false;
        WeaponSkill const& skill = weaponSkills[action - 100];
        if (!player->HasEnoughMoney(1))
        {
            ChatHandler(player->GetSession()).SendSysMessage("You do not have enough money.");
            return OnGossipHello(player, creature);
        }
        player->ModifyMoney(-1);
        GrantWeaponSkill(player, skill);
        ChatHandler(player->GetSession()).PSendSysMessage("Learned {}.", skill.name);
        return OnGossipHello(player, creature);
    }
};

void AddWeaponTrainerScripts()
{
    new weapon_trainer_skills();
    new weapon_trainer_npc();
}
