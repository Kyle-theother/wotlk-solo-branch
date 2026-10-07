#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "CommandScript.h"
#include "ObjectMgr.h"
#include "Item.h"
#include "Config.h"
#include "DatabaseEnv.h"

// ============================================================
//  mod-startergear  (AzerothCore / WotLK 3.3.5a)
//
//  Features:
//    - Toggle module on/off via .conf
//    - Configurable bag item ID and count
//    - Toggle riding skill + mount via .conf
//    - Enable/disable per class via .conf
//    - One-time use per character (DB-tracked)
//    - Funny message on repeated use
//    - Inventory check per GiveItem(); command retryable on bag-full
//    - LOGIC: Heirlooms first, Bags last to ensure space!
// ============================================================

// -------------------------------------------------------
//  CHEST heirlooms
// -------------------------------------------------------
static const uint32 CHEST_CLOTH        = 48691;   // Tattered Dreadmist Robe          (Cloth  - Int)
static const uint32 CHEST_LEATHER_AGI  = 48689;   // Stained Shadowcraft Tunic          (Leather - Agi)
static const uint32 CHEST_LEATHER_INT  = 48687;   // Preened Ironfeather Tunic          (Leather - Int / Druid)
static const uint32 CHEST_MAIL_AGI     = 48677;   // Champion's Deathdealer Breastplate (Mail - Agi)
static const uint32 CHEST_MAIL_INT     = 48683;   // Mystical Vest of Elements         (Mail  - Int)
static const uint32 CHEST_PLATE        = 48685;   // Polished Breastplate of Valor     (Plate - Str)

// -------------------------------------------------------
//  SHOULDER heirlooms
// -------------------------------------------------------
static const uint32 SHOULDER_CLOTH       = 42985; // Tattered Dreadmist Mantle          (Cloth  - Int)
static const uint32 SHOULDER_LEATHER_AGI = 42952; // Stained Shadowcraft Spaulders     (Leather - Agi)
static const uint32 SHOULDER_LEATHER_INT = 42984; // Preened Ironfeather Shoulders     (Leather - Int / Druid)
static const uint32 SHOULDER_MAIL_AGI    = 42950; // Champion Herod's Shoulder          (Mail  - Agi)
static const uint32 SHOULDER_MAIL_INT    = 42951; // Mystical Pauldrons of Elements    (Mail  - Int)
static const uint32 SHOULDER_PLATE       = 42949; // Polished Spaulders of Valor       (Plate - Str)

// -------------------------------------------------------
//  WEAPON heirlooms
// -------------------------------------------------------
// Staves
static const uint32 WEAPON_STAFF_INT    = 42947;  // Dignified Headmaster's Charge     (2H Staff - SpellPow)  Mage/Priest/Lock/Druid
// 2H melee
static const uint32 WEAPON_AXE_2H_STR  = 42943;  // Bloodied Arcanite Reaper          (2H Axe   - Str)        Warrior/Paladin/DK
// 1H melee
static const uint32 WEAPON_MACE_1H_INT = 42948;  // Devout Aurastone Hammer           (1H Mace  - SpellPow/MP5) Shaman/Paladin/Priest/Druid Heal
static const uint32 WEAPON_MACE_1H_AGI = 48716;  // Venerable Mass of McGowan         (1H Mace  - Agi)       Rogue/Enh.Shaman/Warrior/DK
static const uint32 WEAPON_SWORD_1H    = 42945;  // Venerable Dal'Rend's Sacred Charge(1H Sword - AP)        Rogue/Warrior/DK
// Daggers
static const uint32 WEAPON_DAGGER_MH   = 42944;  // Balanced Heartseeker              (Dagger   - Agi)  Rogue MH
static const uint32 WEAPON_DAGGER_OH   = 44091;  // Sharpened Scarlet Kris            (Dagger   - Agi)  Rogue OH
// Ranged
static const uint32 WEAPON_BOW          = 42946;  // Charmed Ancient Bone Bow          (Bow)                Hunter/Warrior/Rogue

// -------------------------------------------------------
//  TRINKET & RING heirlooms
// -------------------------------------------------------
static const uint32 RING_PIRATE        = 50255;  // Dread Pirate Ring                 (+5% XP)
static const uint32 TRINKET_MELEE      = 42991;  // Swift Hand of Justice             (Haste + heal on kill)
static const uint32 TRINKET_CASTER     = 42992;  // Discerning Eye of the Beast       (SpellPow + mana on kill)

// -------------------------------------------------------
//  RIDING spells
// -------------------------------------------------------
static const uint32 SPELL_RIDING_1     = 33388;  // Apprentice Riding
static const uint32 SPELL_RIDING_2     = 33391;  // Journeyman Riding

// -------------------------------------------------------
//  Helper: safely add an item to the player's inventory.
// -------------------------------------------------------
static bool GiveItem(Player* player, uint32 itemId, uint32 count = 1)
{
    if (!itemId)
        return false;

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
    if (!proto)
        return false;

    ItemPosCountVec dest;
    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count);
    if (msg != EQUIP_ERR_OK)
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            "|cffff0000[StarterGear]|r Not enough bag space for: %s – free up some room and try again.",
            proto->Name1.c_str());
        return false;
    }

    Item* item = player->StoreNewItem(dest, itemId, true);
    if (item)
        player->SendNewItem(item, count, true, false);

    return true;
}

// -------------------------------------------------------
//  DB helpers: once-per-character usage check
// -------------------------------------------------------
static bool HasAlreadyUsed(uint32 guidLow)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT 1 FROM custom_startergear_used WHERE guid = {}", guidLow);
    return (result != nullptr);
}

static void MarkAsUsed(uint32 guidLow)
{
    CharacterDatabase.Execute(
        "INSERT IGNORE INTO custom_startergear_used (guid) VALUES ({})", guidLow);
}

// -------------------------------------------------------
//  Command script
// -------------------------------------------------------
using namespace Acore::ChatCommands;

class StarterGearCommandScript : public CommandScript
{
public:
    StarterGearCommandScript() : CommandScript("StarterGearCommandScript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTable =
        {
            { "startergear", HandleStarterGearCommand, SEC_PLAYER, Console::No },
        };
        return commandTable;
    }

    static bool HandleStarterGearCommand(ChatHandler* handler)
    {
        Player* player = handler->GetPlayer();
        if (!player)
            return false;

        // --- Module enabled =) ---
        if (!sConfigMgr->GetOption<bool>("StarterGear.Enable", true))
        {
            handler->PSendSysMessage("|cffff0000[StarterGear]|r This feature is currently disabled.");
            return true;
        }

        uint32 guidLow = player->GetGUID().GetCounter();
        uint8  classId = player->getClass();

        // --- Class enabled? ^^ ---
        bool classEnabled = true;
        switch (classId)
        {
            case CLASS_WARRIOR:     classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.1.Enable",  true); break;
            case CLASS_PALADIN:     classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.2.Enable",  true); break;
            case CLASS_HUNTER:      classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.3.Enable",  true); break;
            case CLASS_ROGUE:       classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.4.Enable",  true); break;
            case CLASS_PRIEST:      classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.5.Enable",  true); break;
            case CLASS_DEATH_KNIGHT:classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.6.Enable",  true); break;
            case CLASS_SHAMAN:      classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.7.Enable",  true); break;
            case CLASS_MAGE:        classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.8.Enable",  true); break;
            case CLASS_WARLOCK:     classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.9.Enable",  true); break;
            case CLASS_DRUID:       classEnabled = sConfigMgr->GetOption<bool>("StarterGear.Class.11.Enable", true); break;
            default: break;
        }

        if (!classEnabled)
        {
            handler->PSendSysMessage("|cffff6600[StarterGear]|r StarterGear is not available for your class.");
            return true;
        }

        // --- Once-per-character check, yeeees baby oO ---
        if (HasAlreadyUsed(guidLow))
        {
        static const char* const jokes[] = {
        "Yo bro, u just used this. Go level urself!",
        "Damn bro, how often do u need starter gear?! Git movin!",
        "Hey bro, wait! U already have starter gear, don't mess with this command!",
        "Bro... seriously? U got ur stuff. Now go kill something!",
        "Lol nice try. One time, one gear. Now get outta here and level!",
        "U think this is a vending machine?! Damn, only once per char, bro!",
        "Starter gear already claimed. What r u even doing here? GO QUEST!",
        "Error 404: More free stuff not found. Try working for it this time!",
        "Bro ur bag is already full of heirlooms, what else do u want, a hug?!",
        "Nope. Nada. Nein. Once per char. Now go touch some mobs!"
        };
        uint32 idx = urand(0, 9);
        handler->SendSysMessage(std::string("|cffff6600[StarterGear]|r ") + jokes[idx]);
        return true;
        }

        handler->PSendSysMessage("|cff00cc44[StarterGear]|r Handing out your starter gear...");

        bool allSuccess = true;

        // --- 1. Riding skill + mount (FIRST) ---
        bool giveRiding = sConfigMgr->GetOption<bool>("StarterGear.GiveRiding", true);
        if (giveRiding)
        {
            if (!player->HasSpell(SPELL_RIDING_1))
                player->learnSpell(SPELL_RIDING_1);
            if (!player->HasSpell(SPELL_RIDING_2))
                player->learnSpell(SPELL_RIDING_2);

            uint32 mountId = sConfigMgr->GetOption<uint32>("StarterGear.MountId", 54811);
            if (mountId)
                if (!GiveItem(player, mountId))
                    allSuccess = false;
        }

        // --- 2. Universal Heirlooms (4 ALL CLASSES) ---
        if (!GiveItem(player, RING_PIRATE))
            allSuccess = false;

        // --- 3. Class-specific heirlooms (2.) ---
        switch (classId)
        {
            case CLASS_MAGE:
            case CLASS_PRIEST:
            case CLASS_WARLOCK:
                if (!GiveItem(player, CHEST_CLOTH))        allSuccess = false;
                if (!GiveItem(player, SHOULDER_CLOTH))     allSuccess = false;
                if (!GiveItem(player, WEAPON_STAFF_INT))   allSuccess = false;
                if (!GiveItem(player, TRINKET_CASTER))     allSuccess = false;
                if (!GiveItem(player, TRINKET_CASTER))     allSuccess = false;
                break;

            case CLASS_DRUID:
                if (!GiveItem(player, CHEST_LEATHER_INT))    allSuccess = false;
                if (!GiveItem(player, CHEST_LEATHER_AGI))    allSuccess = false;
                if (!GiveItem(player, SHOULDER_LEATHER_INT)) allSuccess = false;
                if (!GiveItem(player, SHOULDER_LEATHER_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_STAFF_INT))     allSuccess = false;
                if (!GiveItem(player, TRINKET_CASTER))       allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))        allSuccess = false;
                break;

            case CLASS_ROGUE:
                if (!GiveItem(player, CHEST_LEATHER_AGI))    allSuccess = false;
                if (!GiveItem(player, SHOULDER_LEATHER_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_DAGGER_MH))     allSuccess = false;
                if (!GiveItem(player, WEAPON_DAGGER_OH))     allSuccess = false;
                if (!GiveItem(player, WEAPON_BOW))           allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))        allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))        allSuccess = false;
                break;

            case CLASS_HUNTER:
                if (!GiveItem(player, CHEST_MAIL_AGI))    allSuccess = false;
                if (!GiveItem(player, SHOULDER_MAIL_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_BOW))        allSuccess = false;
                if (!GiveItem(player, WEAPON_AXE_2H_STR)) allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))     allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))     allSuccess = false;
                break;

            case CLASS_SHAMAN:
                if (!GiveItem(player, CHEST_MAIL_AGI))     allSuccess = false;
                if (!GiveItem(player, CHEST_MAIL_INT))     allSuccess = false;
                if (!GiveItem(player, SHOULDER_MAIL_AGI))  allSuccess = false;
                if (!GiveItem(player, SHOULDER_MAIL_INT))  allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_INT)) allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))      allSuccess = false;
                if (!GiveItem(player, TRINKET_CASTER))     allSuccess = false;
                break;

            case CLASS_WARRIOR:
                if (!GiveItem(player, CHEST_PLATE))        allSuccess = false;
                if (!GiveItem(player, SHOULDER_PLATE))     allSuccess = false;
                if (!GiveItem(player, WEAPON_AXE_2H_STR))  allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_BOW))         allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))      allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))      allSuccess = false;
                break;

            case CLASS_PALADIN:
                if (!GiveItem(player, CHEST_PLATE))        allSuccess = false;
                if (!GiveItem(player, SHOULDER_PLATE))     allSuccess = false;
                if (!GiveItem(player, WEAPON_AXE_2H_STR))  allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_INT)) allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))      allSuccess = false;
                if (!GiveItem(player, TRINKET_CASTER))     allSuccess = false;
                break;

            case CLASS_DEATH_KNIGHT:
                if (!GiveItem(player, CHEST_PLATE))        allSuccess = false;
                if (!GiveItem(player, SHOULDER_PLATE))     allSuccess = false;
                if (!GiveItem(player, WEAPON_AXE_2H_STR))  allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_AGI)) allSuccess = false;
                if (!GiveItem(player, WEAPON_MACE_1H_AGI)) allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))      allSuccess = false;
                if (!GiveItem(player, TRINKET_MELEE))      allSuccess = false;
                break;

            default:
                handler->PSendSysMessage("|cffff0000[StarterGear]|r Unknown class – no gear awarded.");
                return true;
        }

        // --- 4. Bags ---
        uint32 bagId = sConfigMgr->GetOption<uint32>("StarterGear.BagId", 41600);
        uint32 bagCount = sConfigMgr->GetOption<uint32>("StarterGear.BagCount", 4);
        for (uint32 i = 0; i < bagCount; ++i)
            if (!GiveItem(player, bagId))
                allSuccess = false;

        // --- Finalize: Only mark as used if EVERY fucking GiveItem call was true ---
        if (allSuccess)
        {
            MarkAsUsed(guidLow);
            handler->PSendSysMessage("|cff00cc44[StarterGear]|r Done! Enjoy your adventure, champion!");
        }
        else
        {
            handler->PSendSysMessage(
                "|cffff6600[StarterGear]|r Some items could not be stored (bag full). "
                "Free up space and use .startergear again – you will NOT be charged a second use.");
        }
        return true;
    }
};

void AddSC_mod_startergear()
{
    new StarterGearCommandScript();
}