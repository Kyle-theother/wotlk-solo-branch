-- Starter items and action bars for every curated extra combination.
-- Gear is copied from a native race of the same class.

DELETE FROM `playercreateinfo_item` WHERE (`race`,`class`) IN ((1,3),(3,7),(3,8),(3,9),(4,8),(4,9),(7,5),(7,3),(11,9),(2,8),(5,3),(5,2),(6,2),(6,5),(8,9),(8,11),(10,1));
INSERT INTO `playercreateinfo_item` (`race`,`class`,`itemid`,`amount`)
SELECT 1, 3, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 3 AND `class` = 3
UNION ALL SELECT 3, 7, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 2 AND `class` = 7
UNION ALL SELECT 3, 8, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 8
UNION ALL SELECT 3, 9, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 9
UNION ALL SELECT 4, 8, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 8
UNION ALL SELECT 4, 9, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 9
UNION ALL SELECT 7, 5, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 5
UNION ALL SELECT 7, 3, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 3 AND `class` = 3
UNION ALL SELECT 11, 9, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 9
UNION ALL SELECT 2, 8, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 8 AND `class` = 8
UNION ALL SELECT 5, 3, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 8 AND `class` = 3
UNION ALL SELECT 5, 2, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 2
UNION ALL SELECT 6, 2, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 2
UNION ALL SELECT 6, 5, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 8 AND `class` = 5
UNION ALL SELECT 8, 9, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 2 AND `class` = 9
UNION ALL SELECT 8, 11, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 6 AND `class` = 11
UNION ALL SELECT 10, 1, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 1;

DELETE FROM `playercreateinfo_action` WHERE (`race`,`class`) IN ((1,3),(3,7),(3,8),(3,9),(4,8),(4,9),(7,5),(7,3),(11,9),(2,8),(5,3),(5,2),(6,2),(6,5),(8,9),(8,11),(10,1));
INSERT INTO `playercreateinfo_action` (`race`,`class`,`button`,`action`,`type`)
SELECT 1, 3, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 3 AND `class` = 3
UNION ALL SELECT 3, 7, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 2 AND `class` = 7
UNION ALL SELECT 3, 8, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 8
UNION ALL SELECT 3, 9, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 9
UNION ALL SELECT 4, 8, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 8
UNION ALL SELECT 4, 9, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 9
UNION ALL SELECT 7, 5, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 5
UNION ALL SELECT 7, 3, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 3 AND `class` = 3
UNION ALL SELECT 11, 9, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 9
UNION ALL SELECT 2, 8, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 8 AND `class` = 8
UNION ALL SELECT 5, 3, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 8 AND `class` = 3
UNION ALL SELECT 5, 2, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 2
UNION ALL SELECT 6, 2, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 2
UNION ALL SELECT 6, 5, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 8 AND `class` = 5
UNION ALL SELECT 8, 9, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 2 AND `class` = 9
UNION ALL SELECT 8, 11, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 6 AND `class` = 11
UNION ALL SELECT 10, 1, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 1;
