-- Custom entries are not in the client Item.dbc, so the bag icon comes from displayid.

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 4336
SET `target`.`displayid` = `source`.`displayid`
WHERE `target`.`entry` = 900011;

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 19160
SET `target`.`displayid` = `source`.`displayid`
WHERE `target`.`entry` = 900012;

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 18231
SET `target`.`displayid` = `source`.`displayid`
WHERE `target`.`entry` = 900013;
