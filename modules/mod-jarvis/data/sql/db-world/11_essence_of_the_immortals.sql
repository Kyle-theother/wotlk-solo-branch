-- Essence of the Immortals. Display falls back to the potion icon if item 34544 has no row.
-- The summoned totem is Jarvis, cloned the same way as the city Jarvis.

DELETE FROM `npc_vendor` WHERE `item` = 900014;
DELETE FROM `item_template` WHERE `entry` = 900014;
DELETE FROM `creature_template_model` WHERE `CreatureID` = 900015;
DELETE FROM `creature_template` WHERE `entry` = 900015;

CREATE TEMPORARY TABLE `tmp_essence` AS SELECT * FROM `item_template` WHERE `entry` = 33447;
UPDATE `tmp_essence` SET
    `entry` = 900014,
    `name` = 'Essence of the Immortals',
    `description` = 'Chaotic energy pulses through this object. Use: grants the raid buffs. In a dungeon, core spells apply raid debuffs and summon Jarvis.',
    `Quality` = 4,
    `Flags` = 134217728,
    `bonding` = 1,
    `class` = 0,
    `subclass` = 1,
    `InventoryType` = 0,
    `ItemLevel` = 80,
    `RequiredLevel` = 1,
    `stackable` = 20,
    `spellid_1` = 48161,
    `spelltrigger_1` = 0,
    `spellcharges_1` = -1,
    `spellppmRate_1` = 0,
    `spellcooldown_1` = 1000,
    `spellcategory_1` = 0,
    `spellcategorycooldown_1` = -1,
    `spellid_2` = 0,
    `spelltrigger_2` = 0,
    `spellcharges_2` = 0,
    `spellid_3` = 0,
    `spelltrigger_3` = 0,
    `spellcharges_3` = 0,
    `spellid_4` = 0,
    `spelltrigger_4` = 0,
    `spellcharges_4` = 0,
    `spellid_5` = 0,
    `spelltrigger_5` = 0,
    `spellcharges_5` = 0,
    `BuyPrice` = 1,
    `SellPrice` = 1,
    `ScriptName` = 'item_essence_of_immortals';
INSERT INTO `item_template` SELECT * FROM `tmp_essence`;
DROP TEMPORARY TABLE `tmp_essence`;

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 34544
SET `target`.`displayid` = `source`.`displayid`
WHERE `target`.`entry` = 900014;

INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
(900010, 30, 900014, 0, 0, 0, 0);

CREATE TEMPORARY TABLE `tmp_jarvis_totem` AS SELECT * FROM `creature_template` WHERE `entry` = 35642;
UPDATE `tmp_jarvis_totem` SET
    `entry` = 900015,
    `name` = 'Jarvis',
    `subname` = 'Totem',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 0,
    `unit_flags` = 768,
    `ScriptName` = 'npc_jarvis_totem';
INSERT INTO `creature_template` SELECT * FROM `tmp_jarvis_totem`;
DROP TEMPORARY TABLE `tmp_jarvis_totem`;

INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`)
SELECT 900015, `Idx`, `CreatureDisplayID`, 0.6, `Probability`
FROM `creature_template_model`
WHERE `CreatureID` = 35642;
