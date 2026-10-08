-- Starter items and action bars for the custom combinations that had none.
DELETE FROM `playercreateinfo_item` WHERE (`race` = 5 AND `class` = 2) OR (`race` = 10 AND `class` = 1);
INSERT INTO `playercreateinfo_item` (`race`, `class`, `itemid`, `amount`)
SELECT 5, 2, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 2;
INSERT INTO `playercreateinfo_item` (`race`, `class`, `itemid`, `amount`)
SELECT 10, 1, `itemid`, `amount` FROM `playercreateinfo_item` WHERE `race` = 1 AND `class` = 1;

DELETE FROM `playercreateinfo_action` WHERE (`race` = 5 AND `class` = 2) OR (`race` = 10 AND `class` = 1);
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 5, 2, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 2;
INSERT INTO `playercreateinfo_action` (`race`, `class`, `button`, `action`, `type`)
SELECT 10, 1, `button`, `action`, `type` FROM `playercreateinfo_action` WHERE `race` = 1 AND `class` = 1;
