-- All weapon skills visible for every race and class.
-- Does not touch CharStartOutfit.dbc or Silithus.
-- Server reads these rows over SkillRaceClassInfo.dbc on worldserver start.
-- Client still needs the matching DBC in a patch MPQ or the skills pane stays empty.
CREATE TABLE IF NOT EXISTS `skillraceclassinfo_dbc` (
  `ID` INT NOT NULL DEFAULT 0,
  `SkillID` INT NOT NULL DEFAULT 0,
  `RaceMask` INT NOT NULL DEFAULT 0,
  `ClassMask` INT NOT NULL DEFAULT 0,
  `Flags` INT NOT NULL DEFAULT 0,
  `MinLevel` INT NOT NULL DEFAULT 0,
  `SkillTierID` INT NOT NULL DEFAULT 0,
  `SkillCostIndex` INT NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

DELETE FROM `skillraceclassinfo_dbc` WHERE `ID` BETWEEN 900001 AND 900015;
INSERT INTO `skillraceclassinfo_dbc` (`ID`,`SkillID`,`RaceMask`,`ClassMask`,`Flags`,`MinLevel`,`SkillTierID`,`SkillCostIndex`) VALUES
(900001, 43, -1, -1, 128, 0, 0, 0), /* Swords */
(900002, 44, -1, -1, 128, 0, 0, 0), /* Axes */
(900003, 45, -1, -1, 128, 0, 0, 0), /* Bows */
(900004, 46, -1, -1, 128, 0, 0, 0), /* Guns */
(900005, 54, -1, -1, 128, 0, 0, 0), /* Maces */
(900006, 55, -1, -1, 128, 0, 0, 0), /* Two-Handed Swords */
(900007, 136, -1, -1, 128, 0, 0, 0), /* Staves */
(900008, 160, -1, -1, 128, 0, 0, 0), /* Two-Handed Maces */
(900009, 172, -1, -1, 128, 0, 0, 0), /* Two-Handed Axes */
(900010, 173, -1, -1, 128, 0, 0, 0), /* Daggers */
(900011, 176, -1, -1, 128, 0, 0, 0), /* Thrown */
(900012, 226, -1, -1, 128, 0, 0, 0), /* Crossbows */
(900013, 228, -1, -1, 128, 0, 0, 0), /* Wands */
(900014, 229, -1, -1, 128, 0, 0, 0), /* Polearms */
(900015, 473, -1, -1, 128, 0, 0, 0); /* Fist Weapons */
