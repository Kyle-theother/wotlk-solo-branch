-- The client will not send a use unless spellid_1 is a spell it knows.
-- Spell 5 is only the button. The item script applies the raid buffs and skips this spell.

UPDATE `item_template`
SET `spellid_1` = 5, `spelltrigger_1` = 0, `spellcharges_1` = 0, `spellcooldown_1` = 0,
    `ScriptName` = 'item_essence_of_immortals'
WHERE `entry` = 900014;
