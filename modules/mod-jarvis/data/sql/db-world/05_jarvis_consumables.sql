-- Naked Bomb and Jarvis consumables at 1 copper.
-- Vendor price is item_template.BuyPrice, so these stock items become 1c everywhere.

DELETE FROM `npc_vendor` WHERE `entry` = 900010 AND `item` IN (41599, 46376, 46377, 46378, 46379, 29529, 43015, 40211, 40212, 900013, 900014);
DELETE FROM `item_template` WHERE `entry` IN (900013, 900014);

UPDATE `item_template` SET `BuyPrice` = 1 WHERE `entry` IN (41599, 46376, 46377, 46378, 46379, 29529, 43015, 40211, 40212);

CREATE TEMPORARY TABLE `tmp_bomb` AS SELECT * FROM `item_template` WHERE `entry` = 38;
UPDATE `tmp_bomb` SET
    `entry` = 900013,
    `name` = 'The Naked Bomb',
    `Quality` = 2,
    `Flags` = 134217728,
    `bonding` = 1,
    `InventoryType` = 0,
    `class` = 0,
    `subclass` = 0,
    `ItemLevel` = 1,
    `RequiredLevel` = 1,
    `spellid_1` = 8012,
    `spelltrigger_1` = 0,
    `spellcharges_1` = 0,
    `spellcooldown_1` = 60000,
    `description` = 'Purges the enemy target, removing 2 beneficial magic effects. Consumes up to 12 Rage/Energy/RP or 5% base mana. 30 yd range.',
    `BuyPrice` = 1,
    `SellPrice` = 1;
INSERT INTO `item_template` SELECT * FROM `tmp_bomb`;
DROP TEMPORARY TABLE `tmp_bomb`;

INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
(900010, 20, 41599, 0, 0, 0, 0),
(900010, 21, 46376, 0, 0, 0, 0),
(900010, 22, 46377, 0, 0, 0, 0),
(900010, 23, 46378, 0, 0, 0, 0),
(900010, 24, 46379, 0, 0, 0, 0),
(900010, 25, 29529, 0, 0, 0, 0),
(900010, 26, 43015, 0, 0, 0, 0),
(900010, 27, 40211, 0, 0, 0, 0),
(900010, 28, 40212, 0, 0, 0, 0),
(900010, 29, 900013, 0, 0, 0, 0);
