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

DELETE FROM `spell_dbc` WHERE `ID` = 900031;
CREATE TEMPORARY TABLE `tmp_spell` AS SELECT * FROM `spell_dbc` WHERE `ID` = 1459;
UPDATE `tmp_spell` SET
    `ID` = 900031,
    `Effect_1` = 6,
    `Effect_2` = 6,
    `Effect_3` = 6,
    `EffectAura_1` = 216,
    `EffectAura_2` = 79,
    `EffectAura_3` = 118,
    `EffectBasePoints_1` = 10000,
    `EffectBasePoints_2` = -51,
    `EffectBasePoints_3` = -51,
    `EffectDieSides_1` = 1,
    `EffectDieSides_2` = 1,
    `EffectDieSides_3` = 1,
    `ImplicitTargetA_1` = 1,
    `ImplicitTargetA_2` = 1,
    `ImplicitTargetA_3` = 1,
    `DurationIndex` = 21,
    `CastingTimeIndex` = 1,
    `RangeIndex` = 1,
    `Attributes` = 0,
    `AttributesEx` = 0,
    `Name_Lang_enUS` = 'Aura of Instant Cast',
    `Name_Lang_enGB` = 'Aura of Instant Cast',
    `NameSubtext_Lang_enUS` = '',
    `Description_Lang_enUS` = 'Toggle. Casts are instant and spell effects are reduced by half.';
INSERT INTO `spell_dbc` SELECT * FROM `tmp_spell`;
DROP TEMPORARY TABLE `tmp_spell`;
