-- Enable the reputation At War checkbox for Alliance, Horde, and their city factions.
UPDATE `faction_dbc`
SET
    `ReputationFlags_1` = (`ReputationFlags_1` | 1 | 2) & ~8 & ~16,
    `ReputationFlags_2` = (`ReputationFlags_2` | 1 | 2) & ~8 & ~16,
    `ReputationFlags_3` = (`ReputationFlags_3` | 1 | 2) & ~8 & ~16,
    `ReputationFlags_4` = (`ReputationFlags_4` | 1 | 2) & ~8 & ~16
WHERE `ID` IN (67, 76, 81, 68, 530, 911, 469, 72, 47, 69, 54, 930);
