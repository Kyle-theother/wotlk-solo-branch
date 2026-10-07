-- ============================================================
--  mod-startergear
--  Sets the required level of the starter mount to 1
--  so it can be used from level 1 onwards.
--
--  Item 54811 = Swift White Steed (Sky Steed)
--  Default RequiredLevel in item_template: 20
--
--  Auto-loaded by AzerothCore DB updater from:
--  modules/mod-startergear/data/sql/db-world/base/
-- ============================================================

UPDATE `item_template`
SET `RequiredLevel` = 1
WHERE `entry` = 54811;
