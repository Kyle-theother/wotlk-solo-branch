-- Negative effect amounts make the client place this aura in the debuff frame.
-- Keep one positive cast-speed aura. The script still halves damage and healing.
UPDATE `spell_dbc`
SET `Effect_1` = 6,
    `Effect_2` = 0,
    `Effect_3` = 0,
    `EffectAura_1` = 216,
    `EffectAura_2` = 0,
    `EffectAura_3` = 0,
    `EffectBasePoints_1` = 10000,
    `EffectBasePoints_2` = 0,
    `EffectBasePoints_3` = 0,
    `EffectDieSides_1` = 1,
    `EffectDieSides_2` = 0,
    `EffectDieSides_3` = 0,
    `ImplicitTargetA_1` = 1,
    `ImplicitTargetA_2` = 0,
    `ImplicitTargetA_3` = 0,
    `Attributes` = 0,
    `AttributesEx` = 0,
    `Name_Lang_enUS` = 'Aura of Instant Cast',
    `Name_Lang_enGB` = 'Aura of Instant Cast',
    `Description_Lang_enUS` = 'Casts are instant and spell effects are reduced by half.',
    `AuraDescription_Lang_enUS` = 'Casts are instant and spell effects are reduced by half.'
WHERE `ID` = 900031;
