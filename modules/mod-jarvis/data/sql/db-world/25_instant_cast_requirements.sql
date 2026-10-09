-- spell_dbc defaults EquippedItemClass to 0, which the server treats as a required weapon.
UPDATE `spell_dbc`
SET `EquippedItemClass` = -1,
    `EquippedItemSubclass` = 0,
    `EquippedItemInvTypes` = 0,
    `RequiresSpellFocus` = 0,
    `CasterAuraState` = 0,
    `TargetAuraState` = 0,
    `ExcludeCasterAuraState` = 0,
    `ExcludeTargetAuraState` = 0,
    `CasterAuraSpell` = 0,
    `TargetAuraSpell` = 0,
    `FacingCasterFlags` = 0,
    `Reagent_1` = 0, `Reagent_2` = 0, `Reagent_3` = 0, `Reagent_4` = 0,
    `Reagent_5` = 0, `Reagent_6` = 0, `Reagent_7` = 0, `Reagent_8` = 0,
    `ReagentCount_1` = 0, `ReagentCount_2` = 0, `ReagentCount_3` = 0, `ReagentCount_4` = 0,
    `ReagentCount_5` = 0, `ReagentCount_6` = 0, `ReagentCount_7` = 0, `ReagentCount_8` = 0,
    `Totem_1` = 0, `Totem_2` = 0
WHERE `ID` = 900031;
