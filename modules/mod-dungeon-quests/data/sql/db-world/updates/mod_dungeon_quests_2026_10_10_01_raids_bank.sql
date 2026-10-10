-- Add the Dungeon Quest Guide to raid entrances and give it banker service.
-- Entrance coordinates are the areatrigger_teleport destinations for each raid.
-- spawnMask 15 covers 10/25 normal and heroic. Existing dungeon spawns are left alone.

UPDATE `creature_template`
SET `npcflag` = 131073, `subname` = 'Expedition Coordinator'
WHERE `entry` = 14999991 AND `ScriptName` = 'npc_dungeon_quest_guide';

INSERT INTO `creature`
    (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`,
     `orientation`, `spawntimesecs`, `curhealth`, `MovementType`)
SELECT 14999991, p.`map`, 15, 1, p.`x`, p.`y`, p.`z`, p.`o`, 300, 1, 0
FROM (
    SELECT 249 AS `map`, 29.1607 AS `x`, -71.3372 AS `y`, -8.18032 AS `z`, 4.58 AS `o` UNION ALL
    SELECT 309, -11916.1, -1230.53, 92.5334, 4.71867 UNION ALL
    SELECT 409, 1091.89, -466.985, -105.084, 3.14159 UNION ALL
    SELECT 469, -7673.03, -1106.08, 396.651, 0.703353 UNION ALL
    SELECT 509, -8429.74, 1512.14, 31.9074, 2.58 UNION ALL
    SELECT 531, -8231.33, 2010.6, 129.331, 0.929912 UNION ALL
    SELECT 532, -11100, -2003.98, 49.8927, 0.577268 UNION ALL
    SELECT 532, -11040.1, -1996.85, 94.6837, 2.20224 UNION ALL
    SELECT 533, 3005.68, -3447.77, 293.93, 4.65 UNION ALL
    SELECT 533, 3019.34, -3434.36, 293.99, 6.27 UNION ALL
    SELECT 533, 3005.9, -3420.58, 294.11, 1.58 UNION ALL
    SELECT 533, 2992.5, -3434.42, 293.94, 3.13 UNION ALL
    SELECT 534, 5163.02, -3428.31, 1627.61, 0.785398 UNION ALL
    SELECT 534, 4259.61, -4233.77, 868.199, 2.53 UNION ALL
    SELECT 544, 187.843, 35.9232, 67.9252, 4.79879 UNION ALL
    SELECT 548, 2.5343, -0.022318, 821.727, 0.004512 UNION ALL
    SELECT 550, -10.8021, -1.15045, -2.42833, 6.22821 UNION ALL
    SELECT 564, 96.4462, 1002.35, -86.9984, 6.15675 UNION ALL
    SELECT 565, 62.7842, 35.462, -3.9835, 1.41844 UNION ALL
    SELECT 568, 120.7, 1776, 43.46, 4.7713 UNION ALL
    SELECT 580, 1790.65, 925.67, 15.15, 3.1 UNION ALL
    SELECT 603, -914.041, -148.98, 463.137, 6.28 UNION ALL
    SELECT 615, 3228.58, 385.86, 65.549, 1.578 UNION ALL
    SELECT 616, 728.055, 1329.03, 275, 5.51524 UNION ALL
    SELECT 624, -505.96, -103.353, 157, 0 UNION ALL
    SELECT 631, 76.8638, 2211.37, 30, 3.14965 UNION ALL
    SELECT 649, 563.61, 80.6815, 395.2, 1.59 UNION ALL
    SELECT 724, 3274, 533.531, 87.665, 3.16
) AS p
WHERE NOT EXISTS (
    SELECT 1 FROM `creature` c
    WHERE c.`id` = 14999991 AND c.`map` = p.`map`
      AND ABS(c.`position_x` - p.`x`) < 0.5
      AND ABS(c.`position_y` - p.`y`) < 0.5
);
