/*
# NPC - Equipment Proficiency #

- This module is based on Azerothcore's Module NPC-All-Mounts (https://github.com/azerothcore/mod-npc-all-mounts).
- Repurposed as SubClass NPC [Equipment Proficiency]

### Description ###
------------------------------------------------------------------------------------------------------------------
- Adds NPCs that will teach all available Armors, Weapons, Skills & Spells Proficiency that is unavailable to the current Class.
- Part 1 of The SubClass Series Module.
- This module focus on Equipment Proficiency (Armors & Weapons)
- Creating SubClasses like character(s).

### To-Do ###
------------------------------------------------------------------------------------------------------------------
- Add Intermediate Skills Proficiency NPC.
- Add Advanced Skills Proficiency NPC.
- Test & Tweak SubClasses Skills & Spells (Balancing).

### Data ###
------------------------------------------------------------------------------------------------------------------
- Type: NPC
- Script: SubClass_NPC
- Config: Yes
- SQL: Yes
- NPC ID: 600001


### Updates ###
------------------------------------------------------------------------------------------------------------------
- Added Warnings/Restrictions when player do not meet the cost and level requirement.
- Added Monetary Check Requirement.
- Added All of the Basic Equipment/Skills Proficiency to the NPC.
- Added Exception For Players With The Learned Abilities/Skills.
- Added Weapons Proficiency.
- Added Armor Proficiency.

*/

#include "Config.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include <string>

bool SubClassAnnounceModule;
bool SubClassEnableAI;

class SubClassConfig : public WorldScript
{
public:
    SubClassConfig() : WorldScript("SubClassConfig_conf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            SubClassAnnounceModule = sConfigMgr->GetOption<bool>("SubClassNPC.Announce", 1);
            SubClassEnableAI = sConfigMgr->GetOption<bool>("SubClassNPC.EnableAI", 1);
        }
    }
};

class SubClassAnnounce : public PlayerScript
{

public:

    SubClassAnnounce() : PlayerScript("SubClassAnnounce") {}

    void OnLogin(Player* player)
    {
        // Announce Module
        if (SubClassAnnounceModule)
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00SubClassNPC |rmodule.");
        }
    }
};


static void LearnProficiency(Player* player, uint32 spellId, uint16 skillId)
{
    if (!player->HasSpell(spellId))
        player->learnSpell(spellId, false);
    if (skillId && player->GetSkillValue(skillId) == 0)
        player->SetSkill(skillId, 0, 1, 1);
}


SubClass_NPC::Proficiency const SubClass_NPC::profs[] =
{
    { 9077, 414, 0, "Leather", true },
    { 8737, 413, 0, "Mail", true },
    { 750, 293, 0, "Plate", true },
    { 9116, 433, 107, "Shield", true },
    { 674, 0, 0, "Dual Wield", false },
    { 196, 44, 0, "1H Axe", false },
    { 197, 172, 0, "2H Axe", false },
    { 201, 43, 0, "1H Sword", false },
    { 202, 55, 0, "2H Sword", false },
    { 198, 54, 0, "1H Mace", false },
    { 199, 160, 0, "2H Mace", false },
    { 200, 229, 0, "Polearm", false },
    { 1180, 173, 0, "Dagger", false },
    { 15590, 473, 0, "Fist", false },
    { 227, 136, 0, "Staff", false },
    { 5009, 228, 5019, "Wand", false },
    { 2567, 176, 0, "Thrown", false },
    { 2764, 0, 0, "Throw", false },
    { 264, 45, 3018, "Bow", false },
    { 5011, 226, 3018, "Crossbow", false },
    { 266, 46, 3018, "Gun", false },
    { 0, 0, 0, "", false }
};

class SubClass_NPC : public CreatureScript
{

public:

    SubClass_NPC() : CreatureScript("SubClass_NPC") {}

    struct Proficiency
    {
        uint32 spellId;
        uint16 skillId;
        uint32 extraSpell;
        char const* name;
        bool armor;
    };

    static Proficiency const profs[];

    void ShowProficiencies(Player* player, Creature* creature, bool armor)
    {
        ClearGossipMenuFor(player);
        uint32 shown = 0;
        for (uint32 i = 0; profs[i].spellId; ++i)
        {
            if (profs[i].armor != armor || player->HasSpell(profs[i].spellId))
                continue;
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, profs[i].name, armor ? 10 : 11, i);
            ++shown;
        }
        if (!shown)
            ChatHandler(player->GetSession()).SendSysMessage(armor ? "You already know every armor proficiency." : "You already know every weapon proficiency.");
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Go back", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, 600001, creature->GetGUID());
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Armor", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Weapons", GOSSIP_SENDER_MAIN, 2);
        SendGossipMenuFor(player, 600001, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (sender == GOSSIP_SENDER_MAIN && action == 99)
            return OnGossipHello(player, creature);
        if (sender == GOSSIP_SENDER_MAIN && action == 1)
        {
            ShowProficiencies(player, creature, true);
            return true;
        }
        if (sender == GOSSIP_SENDER_MAIN && action == 2)
        {
            ShowProficiencies(player, creature, false);
            return true;
        }
        if ((sender == 10 || sender == 11) && action < 32 && profs[action].spellId)
        {
            Proficiency const& prof = profs[action];
            if (!player->HasEnoughMoney(1))
            {
                ChatHandler(player->GetSession()).SendSysMessage("You do not have enough money.");
                ShowProficiencies(player, creature, sender == 10);
                return true;
            }
            player->ModifyMoney(-1);
            LearnProficiency(player, prof.spellId, prof.skillId);
            if (prof.extraSpell)
                LearnProficiency(player, prof.extraSpell, 0);
            ChatHandler(player->GetSession()).PSendSysMessage("Learned {}.", prof.name);
            ShowProficiencies(player, creature, sender == 10);
            return true;
        }
        CloseGossipMenuFor(player);
        return true;
    }

    struct NPC_PassiveAI : public ScriptedAI
    {
        NPC_PassiveAI(Creature * creature) : ScriptedAI(creature) { }

        uint32 Choice;
        uint32 MessageTimer;

        // Called once when client is loaded
        void Reset()
        {
            MessageTimer = urand(60000, 180000); // 1-3 minutes
        }

        // Called at World update tick
        void UpdateAI(const uint32 diff)
        {
            if (SubClassEnableAI)
            {
                if (MessageTimer <= diff)
                {
                    // Make a random message choice
                    Choice = urand(1, 3);

                    switch (Choice)
                    {
                    case 1:
                    {
                        me->Say("I can teach you anything and everything about equipment!", LANG_UNIVERSAL);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        MessageTimer = urand(60000, 180000);
                        break;
                    }
                    case 2:
                    {
                        me->Say("You should try to gear up differently!!", LANG_UNIVERSAL);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        MessageTimer = urand(60000, 180000);
                        break;
                    }
                    case 3:
                    {
                        me->Say("Wield any weapon that you have always wanted!.", LANG_UNIVERSAL);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        MessageTimer = urand(60000, 180000);
                        break;
                    }
                    default:
                    {
                        me->Say("Have you ever wanted to wear different type of armor?.", LANG_UNIVERSAL);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        MessageTimer = urand(60000, 180000);
                        break;
                    }
                    }
                }
                else { MessageTimer -= diff; }
            }
        };
    };

    // CREATURE AI
    CreatureAI * GetAI(Creature * creature) const override
    {
        return new NPC_PassiveAI(creature);
    }
};

void AddSubClassNPCScripts()
{
    new SubClassConfig();
    new SubClassAnnounce();
    new SubClass_NPC();
}
