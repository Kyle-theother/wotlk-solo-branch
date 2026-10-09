-- The scroll needs a use spell or the client never sends the use packet.
UPDATE `item_template`
SET `class` = 0,
    `subclass` = 0,
    `spellid_1` = 900031,
    `spelltrigger_1` = 0,
    `spellcharges_1` = -1,
    `spellcategory_1` = 0,
    `ScriptName` = 'item_instant_cast_scroll',
    `description` = 'Teaches Aura of Instant Cast.'
WHERE `entry` = 900030;
