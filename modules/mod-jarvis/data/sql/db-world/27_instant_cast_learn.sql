-- This core teaches a spell only when spell 1 is 483 and spell 2 is the spell to learn.
UPDATE `item_template`
SET `class` = 9,
    `subclass` = 0,
    `spellid_1` = 483,
    `spelltrigger_1` = 0,
    `spellcharges_1` = 1,
    `spellid_2` = 900031,
    `spelltrigger_2` = 0,
    `spellcharges_2` = 0,
    `description` = 'Teaches Aura of Instant Cast.',
    `ScriptName` = ''
WHERE `entry` = 900030;
