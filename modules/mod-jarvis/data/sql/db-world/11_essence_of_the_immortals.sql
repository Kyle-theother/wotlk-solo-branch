-- Essence of the Immortals, cloned from a potion and displayed as item 34544.

DELETE FROM `npc_vendor` WHERE `item` = 900014;
DELETE FROM `item_template` WHERE `entry` = 900014;
DELETE FROM `creature` WHERE `id1` = 900015;
DELETE FROM `creature_template_model` WHERE `CreatureID` = 900015;
DELETE FROM `creature_template` WHERE `entry` = 900015;

CREATE TEMPORARY TABLE `tmp_essence` AS SELECT * FROM `item_template` WHERE `entry` = 33447;
UPDATE `tmp_essence` SET
    `entry` = 900014,
    `name` = 'Essence of the Immortals',
    `description` = 'Chaotic energy pulses through this object. Use: grants the raid buffs. In a dungeon, core spells apply raid debuffs and summon Roy.',
    `Quality` = 4,
    `Flags` = 134217728,
    `bonding` = 1,
    `class` = 0,
    `subclass` = 1,
    `InventoryType` = 0,
    `displayid` = (SELECT `displayid` FROM `item_template` WHERE `entry` = 34544),
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

INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
(900010, 30, 900014, 0, 0, 0, 0);

INSERT INTO `creature_template` (`entry`, `name`, `subname`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `unit_class`, `unit_flags`, `type`, `type_flags`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(900015, 'Roy', 'Totem', 80, 80, 35, 0, 1, 1.14286, 0.6, 0, 1, 33554690, 11, 0, 1, 2, 'npc_roy_totem');

INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`) VALUES
(900015, 0, 29354, 0.6, 1);
