DELETE FROM `spell_dbc` WHERE `ID` = 900031;
INSERT INTO `spell_dbc`
(`ID`, `Attributes`, `CastingTimeIndex`, `DurationIndex`, `RangeIndex`,
 `Effect_1`, `Effect_2`, `Effect_3`,
 `EffectDieSides_1`, `EffectDieSides_2`, `EffectDieSides_3`,
 `EffectBasePoints_1`, `EffectBasePoints_2`, `EffectBasePoints_3`,
 `ImplicitTargetA_1`, `ImplicitTargetA_2`, `ImplicitTargetA_3`,
 `EffectAura_1`, `EffectAura_2`, `EffectAura_3`,
 `Name_Lang_enUS`, `Name_Lang_enGB`, `Description_Lang_enUS`)
VALUES
(900031, 0, 1, 21, 1,
 6, 6, 6,
 1, 1, 1,
 10000, -51, -51,
 1, 1, 1,
 216, 79, 118,
 'Aura of Instant Cast', 'Aura of Instant Cast', 'Toggle. Casts are instant and spell effects are reduced by half.');
