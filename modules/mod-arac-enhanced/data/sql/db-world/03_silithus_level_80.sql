-- Silithus (zone 1377) as a level 80 zone.
-- Only creature templates that spawn in Silithus and nowhere else are changed,
-- so shared critters and world bosses used in other zones stay put.
-- Quest XP follows QuestLevel. MinLevel 77 lets a fresh 80 pick the chain up
-- on the way in; the quests themselves are level 80.

UPDATE `creature_template` ct
INNER JOIN (
    SELECT `id1` AS `entry`
    FROM `creature`
    GROUP BY `id1`
    HAVING SUM(`zoneId` = 1377) > 0
       AND SUM(`zoneId` <> 1377) = 0
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
