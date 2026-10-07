struct JarvisSpell { char const* name; uint32 id; uint32 gold; uint8 cls; };
static char const* spellClasses[] = { "Death Knight", "Druid", "Hunter", "Mage", "Paladin", "Priest", "Rogue", "Shaman", "Warlock", "Warrior" };
static uint8 const spellClassIds[] = { CLASS_DEATH_KNIGHT, CLASS_DRUID, CLASS_HUNTER, CLASS_MAGE, CLASS_PALADIN, CLASS_PRIEST, CLASS_ROGUE, CLASS_SHAMAN, CLASS_WARLOCK, CLASS_WARRIOR };
static JarvisSpell const borrowed[] =
{
    { "Anti-Magic Shell", 48707, 500, CLASS_DEATH_KNIGHT }, { "Death Grip", 49576, 500, CLASS_DEATH_KNIGHT },
    { "Innervate", 29166, 500, CLASS_DRUID }, { "Mark of the Wild", 48469, 300, CLASS_DRUID }, { "Rebirth", 48477, 800, CLASS_DRUID },
    { "Arcane Intellect", 42995, 300, CLASS_MAGE }, { "Blink", 1953, 500, CLASS_MAGE }, { "Ice Block", 45438, 800, CLASS_MAGE },
    { "Blessing of Kings", 20217, 500, CLASS_PALADIN }, { "Blessing of Might", 48932, 500, CLASS_PALADIN }, { "Lay on Hands", 48788, 500, CLASS_PALADIN },
    { "Power Word: Fortitude", 48161, 300, CLASS_PRIEST }, { "Power Word: Shield", 48066, 500, CLASS_PRIEST },
    { "Sprint", 11305, 400, CLASS_ROGUE }, { "Stealth", 1784, 400, CLASS_ROGUE },
    { "Bloodlust", 2825, 800, CLASS_SHAMAN }, { "Heroism", 32182, 800, CLASS_SHAMAN }, { "Wind Shear", 57994, 500, CLASS_SHAMAN },
    { "Healthstone", 47878, 300, CLASS_WARLOCK }, { "Soulstone", 47883, 500, CLASS_WARLOCK },
    { "Battle Shout", 47436, 300, CLASS_WARRIOR }, { "Intervene", 3411, 500, CLASS_WARRIOR }
};
