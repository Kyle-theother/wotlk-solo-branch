-- The At War checkbox is available only when the first reputation race mask is all races.
UPDATE `faction_dbc`
SET
    `ReputationRaceMask_1` = 1791,
    `ReputationFlags_1` = (`ReputationFlags_1` | 1 | 2) & ~8 & ~16,
    `ReputationFlags_2` = (`ReputationFlags_2` | 1 | 2) & ~8 & ~16,
    `ReputationFlags_3` = (`ReputationFlags_3` | 1 | 2) & ~8 & ~16,
    `ReputationFlags_4` = (`ReputationFlags_4` | 1 | 2) & ~8 & ~16
WHERE `ID` IN (67, 76, 81, 68, 530, 911, 469, 72, 47, 69, 54, 930);
