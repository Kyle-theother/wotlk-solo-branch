-- Bag icon for custom entry 900014 comes from item_template.displayid.
-- Use the real Essence of the Immortals display (item 34544, icon spell_shadow_manafeed).

UPDATE `item_template` AS `target`
JOIN `item_template` AS `source` ON `source`.`entry` = 34544
SET `target`.`displayid` = `source`.`displayid`,
    `target`.`Quality` = 4,
    `target`.`name` = 'Essence of the Immortals'
WHERE `target`.`entry` = 900014;
