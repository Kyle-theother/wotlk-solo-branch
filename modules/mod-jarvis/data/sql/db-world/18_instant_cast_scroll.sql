-- Scroll that teaches Aura of Instant Cast. Jarvis sells it for 1 copper.
DELETE FROM `npc_vendor` WHERE `item` = 900030;
DELETE FROM `item_template` WHERE `entry` = 900030;

CREATE TEMPORARY TABLE `tmp_scroll` AS SELECT * FROM `item_template` WHERE `entry` = 3012;
UPDATE `tmp_scroll` SET
    `entry` = 900030,
    `name` = 'Scroll of Aura of Instant Cast',
    `description` = 'Teaches Aura of Instant Cast. Use again to toggle it. Instant casts, spell effects reduced by half.',
    `Quality` = 3,
    `bonding` = 1,
    `maxcount` = 1,
    `spellid_1` = 0,
    `spellcharges_1` = 0,
    `BuyPrice` = 1,
    `SellPrice` = 1,
    `ScriptName` = 'item_instant_cast_scroll';
INSERT INTO `item_template` SELECT * FROM `tmp_scroll`;
DROP TEMPORARY TABLE `tmp_scroll`;

INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(900010, 900030, 0, 0, 0);

DELETE FROM `spell_dbc` WHERE `Id` = 900031;
CREATE TEMPORARY TABLE `tmp_spell` AS SELECT * FROM `spell_dbc` WHERE `Id` = 1459;
UPDATE `tmp_spell` SET `Id` = 900031, `Effect1` = 6, `Effect2` = 0, `Effect3` = 0, `EffectBasePoints1` = 0, `EffectBasePoints2` = 0, `EffectBasePoints3` = 0, `ImplicitTargetA1` = 1, `ImplicitTargetA2` = 0, `ImplicitTargetA3` = 0, `DurationIndex` = 21, `CastingTimeIndex` = 1, `RangeIndex` = 1, `Attributes` = 0, `AttributesEx` = 0, `SpellName` = 'Aura of Instant Cast', `Rank` = '', `Description` = 'Toggle. Casts are instant and spell effects are reduced by half.';
INSERT INTO `spell_dbc` SELECT * FROM `tmp_spell`;
DROP TEMPORARY TABLE `tmp_spell`;
