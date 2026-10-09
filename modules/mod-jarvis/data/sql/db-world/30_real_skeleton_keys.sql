-- Replace the custom key copies with the real skeleton keys.
DELETE FROM `npc_vendor` WHERE `entry` = 900010 AND `item` IN (900020, 900021, 900022, 900023, 15869, 15870, 15871, 15872, 43853, 43854);
INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(900010, 15869, 0, 0, 0),
(900010, 15870, 0, 0, 0),
(900010, 15871, 0, 0, 0),
(900010, 15872, 0, 0, 0),
(900010, 43854, 0, 0, 0),
(900010, 43853, 0, 0, 0);
