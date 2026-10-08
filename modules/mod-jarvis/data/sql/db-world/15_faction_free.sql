-- Faction-Free updates that match this core.
-- The module's creature inserts use the removed scale column and creature.id1, so those are not imported.
-- PlayerUpdates.cpp is not replaced because it does not compile on this core.

UPDATE `acore_world`.`quest_template` SET `AllowableRaces` = 1791 WHERE `AllowableRaces` = 1101 OR `AllowableRaces` = 690;
UPDATE `acore_world`.`areatrigger_tavern` SET `faction` = 6 WHERE `faction` != 6;
UPDATE `acore_world`.`playercreateinfo_skills` SET `raceMask`=1791 WHERE `raceMask`!=1791 AND `comment` LIKE "Language%";
UPDATE `acore_world`.`broadcast_text` SET `LanguageID` = 0 WHERE `LanguageID` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`creature_text` SET `Language` = 0 WHERE `Language` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang0` = 0 WHERE `lang0` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang1` = 0 WHERE `lang1` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang2` = 0 WHERE `lang2` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang3` = 0 WHERE `lang3` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang4` = 0 WHERE `lang4` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang5` = 0 WHERE `lang5` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang6` = 0 WHERE `lang6` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`npc_text` SET `lang7` = 0 WHERE `lang7` IN (1,2,3,6,7,10,13,14,33,35);
UPDATE `acore_world`.`item_template` SET `FlagsExtra` = 0 WHERE `FlagsExtra` IN (1, 2);
UPDATE `acore_world`.`item_template` SET `FlagsExtra` = 8192 WHERE `FlagsExtra` IN (8193, 8194);
UPDATE `acore_world`.`item_template` SET `AllowableRace` = -1;
UPDATE `creature_template` SET faction = 85 WHERE faction IN (83, 1734, 106, 1735, 1495, 1637);
UPDATE `creature_template` SET faction = 11 WHERE faction IN (53, 56, 84, 1733, 210, 1732);
UPDATE `creature_template` SET faction = 14 WHERE entry IN (36950,38406,38685,38686,36957,38404,38679,38680,36960,38262,38683,38684,36961,38261,38691,38692,36968,38403,38675,38676,36969,38408,38689,38690,36978,38407,38687,38688,36982,38405,38681,38682,37116,38256,38693,38694,37117,38257,38677,38678);
UPDATE `acore_world`.`creature_template` SET `faction` = 7 WHERE `entry` IN (36776, 36774);
