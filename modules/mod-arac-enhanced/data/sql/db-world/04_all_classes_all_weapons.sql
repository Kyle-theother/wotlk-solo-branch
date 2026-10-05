-- Every class can equip every weapon type.
-- classMask 1535 is all playable classes (no class 10).
-- raceMask 0 is all races.
-- New characters learn the skills from playercreateinfo_skills.
-- Existing characters need the acore_characters statement, with max 400
-- so the skill can reach the level 80 cap. max 1 would make every swing miss.

UPDATE `item_template`
SET `AllowableClass` = -1
WHERE `class` = 2
  AND `AllowableClass` <> -1;

INSERT IGNORE INTO `playercreateinfo_skills` (`raceMask`, `classMask`, `skill`, `rank`, `comment`) VALUES
(0, 1535, 43, 1, 'All classes - Swords'),
(0, 1535, 44, 1, 'All classes - Axes'),
(0, 1535, 45, 1, 'All classes - Bows'),
(0, 1535, 46, 1, 'All classes - Guns'),
(0, 1535, 54, 1, 'All classes - Maces'),
(0, 1535, 55, 1, 'All classes - Two-Handed Swords'),
(0, 1535, 136, 1, 'All classes - Staves'),
(0, 1535, 160, 1, 'All classes - Two-Handed Maces'),
(0, 1535, 172, 1, 'All classes - Two-Handed Axes'),
(0, 1535, 173, 1, 'All classes - Daggers'),
(0, 1535, 176, 1, 'All classes - Thrown'),
(0, 1535, 226, 1, 'All classes - Crossbows'),
(0, 1535, 227, 1, 'All classes - Wands'),
(0, 1535, 228, 1, 'All classes - Polearms'),
(0, 1535, 229, 1, 'All classes - Fist Weapons');

-- Run this against acore_characters after the world update has applied:
-- INSERT IGNORE INTO `character_skills` (`guid`, `skill`, `value`, `max`)
-- SELECT c.`guid`, s.`skill`, 1, 400
-- FROM `characters` c
-- JOIN (
--     SELECT 43 AS skill UNION ALL SELECT 44 UNION ALL SELECT 45 UNION ALL SELECT 46
--     UNION ALL SELECT 54 UNION ALL SELECT 55 UNION ALL SELECT 136 UNION ALL SELECT 160
--     UNION ALL SELECT 172 UNION ALL SELECT 173 UNION ALL SELECT 176 UNION ALL SELECT 226
--     UNION ALL SELECT 227 UNION ALL SELECT 228 UNION ALL SELECT 229
-- ) s
-- WHERE c.`deleteDate` IS NULL;
