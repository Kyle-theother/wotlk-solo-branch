-- Silver, gold, fel, and titan keys. Using one near a key-locked door opens it.
DELETE FROM `npc_vendor` WHERE `item` IN (900020, 900021, 900022, 900023);
DELETE FROM `item_template` WHERE `entry` IN (900020, 900021, 900022, 900023);

CREATE TEMPORARY TABLE `tmp_key` AS SELECT * FROM `item_template` WHERE `entry` = 7146;
UPDATE `tmp_key` SET `entry` = 900020, `name` = 'Silver Key', `description` = 'Opens a nearby key-locked door.', `Quality` = 2, `bonding` = 1, `BuyPrice` = 1, `SellPrice` = 1, `spellid_1` = 0, `ScriptName` = 'item_dungeon_key';
INSERT INTO `item_template` SELECT * FROM `tmp_key`;
UPDATE `tmp_key` SET `entry` = 900021, `name` = 'Gold Key', `Quality` = 3;
INSERT INTO `item_template` SELECT * FROM `tmp_key`;
UPDATE `tmp_key` SET `entry` = 900022, `name` = 'Fel Key', `Quality` = 3;
INSERT INTO `item_template` SELECT * FROM `tmp_key`;
UPDATE `tmp_key` SET `entry` = 900023, `name` = 'Titan Key', `Quality` = 4;
INSERT INTO `item_template` SELECT * FROM `tmp_key`;
DROP TEMPORARY TABLE `tmp_key`;

INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(900010, 900020, 0, 0, 0),
(900010, 900021, 0, 0, 0),
(900010, 900022, 0, 0, 0),
(900010, 900023, 0, 0, 0);
