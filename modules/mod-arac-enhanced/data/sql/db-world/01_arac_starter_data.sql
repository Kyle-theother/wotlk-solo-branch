-- Spawn new race/class combos in that race's normal starting zone.
-- Copies position, action bar, and starter items from an existing class of the same race.
-- Undead paladin is race 5, class 2.

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 5, 2, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 5 AND `class` = 8 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 5, 3, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 5 AND `class` = 8 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 1, 3, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 1 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 3, 7, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 3 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 3, 8, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 3 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 3, 9, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 3 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 4, 8, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 4 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 4, 9, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 4 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 7, 5, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 7 AND `class` = 8 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 7, 3, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 7 AND `class` = 8 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 11, 9, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 11 AND `class` = 8 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 2, 8, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 2 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 6, 2, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 6 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 6, 5, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 6 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 8, 9, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 8 AND `class` = 8 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 8, 11, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 8 AND `class` = 1 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo` (`race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation`)
SELECT 10, 1, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo` WHERE `race` = 10 AND `class` = 2 LIMIT 1;

INSERT IGNORE INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 5, 2, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 2;

INSERT IGNORE INTO `playercreateinfo_item` (`race`, `class`, `itemid`, `amount`)
SELECT 5, 2, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 2;
