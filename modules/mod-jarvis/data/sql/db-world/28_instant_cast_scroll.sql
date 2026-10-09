-- Rebuild the teaching scroll from a recipe item. This core teaches only when spell 1 is 483 and spell 2 is the granted spell.
DELETE FROM `npc_vendor` WHERE `item` = 900030;
DELETE FROM `item_template` WHERE `entry` = 900030;

CREATE TEMPORARY TABLE `tmp_scroll` AS SELECT * FROM `item_template` WHERE `entry` = 728;
UPDATE `tmp_scroll` SET
    `entry` = 900030,
    `class` = 9,
    `subclass` = 0,
    `name` = 'Scroll of Aura of Instant Cast',
    `description` = 'Teaches Aura of Instant Cast.',
    `displayid` = 132935,
    `Quality` = 3,
    `ItemLevel` = 1,
    `RequiredLevel` = 0,
    `maxcount` = 0,
    `stackable` = 20,
    `bonding` = 1,
    `BuyPrice` = 1,
    `SellPrice` = 1,
    `spellid_1` = 483,
    `spelltrigger_1` = 0,
    `spellcharges_1` = -1,
    `spellid_2` = 900031,
    `spelltrigger_2` = 6,
    `spellcharges_2` = 0,
    `spellid_3` = 0,
    `spellid_4` = 0,
    `spellid_5` = 0,
    `ScriptName` = '';
INSERT INTO `item_template` SELECT * FROM `tmp_scroll`;
DROP TEMPORARY TABLE `tmp_scroll`;

INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(900010, 900030, 0, 0, 0);
