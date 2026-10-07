-- Martin Fury and Competitor's Tabard. Account-bound, +1 all stats, item level 5.
-- Shirt use is Dispel Magic (527). Tabard use is Wind Shear (57994).
-- The yellow tooltip text is the client spell. The description is the fallback.

DELETE FROM `npc_vendor` WHERE `item` IN (900011, 900012);
DELETE FROM `item_template` WHERE `entry` IN (900011, 900012);

CREATE TEMPORARY TABLE `tmp_martin` AS
SELECT * FROM `item_template` WHERE `entry` = 38;

UPDATE `tmp_martin`
SET
    `entry` = 900011,
    `name` = 'Martin Fury',
    `Quality` = 2,
    `Flags` = 134217728,
    `bonding` = 1,
    `InventoryType` = 4,
    `ItemLevel` = 5,
    `RequiredLevel` = 1,
    `stat_type1` = 4, `stat_value1` = 1,
    `stat_type2` = 3, `stat_value2` = 1,
    `stat_type3` = 7, `stat_value3` = 1,
    `stat_type4` = 5, `stat_value4` = 1,
    `stat_type5` = 6, `stat_value5` = 1,
    `spellid_1` = 527,
    `spelltrigger_1` = 0,
    `spellcharges_1` = 0,
    `spellcooldown_1` = 60000,
    `description` = 'Removes 1 magic, curse, poison or disease effect on nearby group members instantly and every 3 seconds for 6 seconds. Consumes up to 12 Rage/Energy/RP or 5% base mana.',
    `BuyPrice` = 10000,
    `SellPrice` = 2500;

INSERT INTO `item_template` SELECT * FROM `tmp_martin`;
DROP TEMPORARY TABLE `tmp_martin`;

CREATE TEMPORARY TABLE `tmp_tabard` AS
SELECT * FROM `item_template` WHERE `entry` = 5976;

UPDATE `tmp_tabard`
SET
    `entry` = 900012,
    `name` = 'Competitor''s Tabard',
    `Quality` = 2,
    `Flags` = 134217728,
    `bonding` = 1,
    `InventoryType` = 19,
    `ItemLevel` = 5,
    `RequiredLevel` = 1,
    `stat_type1` = 4, `stat_value1` = 1,
    `stat_type2` = 3, `stat_value2` = 1,
    `stat_type3` = 7, `stat_value3` = 1,
    `stat_type4` = 5, `stat_value4` = 1,
    `stat_type5` = 6, `stat_value5` = 1,
    `spellid_1` = 57994,
    `spelltrigger_1` = 0,
    `spellcharges_1` = 0,
    `spellcooldown_1` = 60000,
    `description` = 'Instantly blasts the target with a gust of wind, causing no damage but interrupting spellcasting and preventing any spell in that school from being cast for 2 sec. Consumes up to 12 Rage/Energy/RP or 5% base mana. 25 yd range.',
    `BuyPrice` = 10000,
    `SellPrice` = 2500;

INSERT INTO `item_template` SELECT * FROM `tmp_tabard`;
DROP TEMPORARY TABLE `tmp_tabard`;

INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
(900010, 10, 900011, 0, 0, 0, 0),
(900010, 11, 900012, 0, 0, 0, 0);
