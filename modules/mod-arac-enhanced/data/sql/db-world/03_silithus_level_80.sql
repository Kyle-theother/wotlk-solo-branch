-- Silithus as a level 80 zone.
-- zoneId is 0 during db-import (the worldserver fills it later), so this matches
-- the Kalimdor bounding box for Silithus as well as a populated zoneId.
-- Only templates that do not also spawn outside that box are changed.

UPDATE `creature_template` ct
INNER JOIN (
    SELECT `id1` AS `entry`
    FROM `creature`
    GROUP BY `id1`
    HAVING SUM(
            `zoneId` = 1377
            OR (`map` = 1 AND `position_x` BETWEEN -8400 AND -6000 AND `position_y` BETWEEN -100 AND 2700)
           ) > 0
       AND SUM(
            NOT (
                `zoneId` = 1377
                OR (`map` = 1 AND `position_x` BETWEEN -8400 AND -6000 AND `position_y` BETWEEN -100 AND 2700)
            )
           ) = 0
) silithus_only ON silithus_only.`entry` = ct.`entry`
SET
    ct.`minlevel` = 80,
    ct.`maxlevel` = 80,
    ct.`HealthModifier` = ct.`HealthModifier` * 6,
    ct.`DamageModifier` = ct.`DamageModifier` * 3
WHERE ct.`minlevel` > 0
  AND ct.`minlevel` < 80;

UPDATE `quest_template`
SET
    `QuestLevel` = 80,
    `MinLevel` = 77
WHERE `QuestSortID` = 1377
  AND `QuestLevel` > 0
  AND `QuestLevel` < 80;
