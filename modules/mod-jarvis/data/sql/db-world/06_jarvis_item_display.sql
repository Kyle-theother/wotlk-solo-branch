-- Custom entries are not in the client Item.dbc, so the bag icon comes from displayid.
-- These point at real Wrath display rows. Essence also needs a spell or the client shows no Use button.

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 4336
SET `target`.`displayid` = `source`.`displayid`
WHERE `target`.`entry` = 900011;

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 19160
SET `target`.`displayid` = `source`.`displayid`
WHERE `target`.`entry` = 900012;

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 40211
SET `target`.`displayid` = `source`.`displayid`,
    `target`.`class` = 0,
    `target`.`subclass` = 0,
    `target`.`spellid_1` = 48470,
    `target`.`spelltrigger_1` = 0,
    `target`.`spellcharges_1` = -1,
    `target`.`spellcooldown_1` = 0,
    `target`.`ScriptName` = 'item_essence_of_immortals'
WHERE `target`.`entry` = 900014;

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 18231
SET `target`.`displayid` = `source`.`displayid`
WHERE `target`.`entry` = 900013;
