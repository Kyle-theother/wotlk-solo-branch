-- Cleanup rogue custom race data (e.g. race >= 12 or raceMask >= 2048)
-- Prevents AzerothCore startup errors:
-- "Wrong race 13 in playercreateinfo table, ignoring."
-- "Wrong race mask 4096 in playercreateinfo_skills table, ignoring."
-- "Wrong race 13 in playercreateinfo_action table, ignoring."
--
-- Do not drop charstartoutfit_dbc. The worldserver loads that table during
-- Initialize Data Stores and aborts if it is missing.

DELETE FROM `playercreateinfo` WHERE `race` >= 12;
DELETE FROM `playercreateinfo_action` WHERE `race` >= 12;
DELETE FROM `playercreateinfo_item` WHERE `race` >= 12;
DELETE FROM `playercreateinfo_skills` WHERE `raceMask` >= 2048;
DELETE FROM `playercreateinfo_spell_custom` WHERE `racemask` >= 2048;
