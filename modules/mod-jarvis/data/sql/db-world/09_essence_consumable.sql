-- Rebuild Essence of Immortals from a real consumable so the client shows Use.

DELETE FROM `item_template` WHERE `entry` = 900014;
CREATE TEMPORARY TABLE `tmp_essence` AS SELECT * FROM `item_template` WHERE `entry` = 33447;
UPDATE `tmp_essence` SET
    `entry` = 900014,
    `name` = 'Essence of Immortals',
    `description` = 'Use: grants the raid buffs and arms attack debuffs for one hour.',
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
