-- Roy, a city service NPC. Entry 900001 is in the custom range.
-- Copied from Stormwind City Guard (68) so required columns stay valid.

DELETE FROM `creature` WHERE `id1` = 900001;
DELETE FROM `npc_vendor` WHERE `entry` = 900001;
DELETE FROM `creature_template` WHERE `entry` = 900001;

CREATE TEMPORARY TABLE `tmp_roy` AS
SELECT * FROM `creature_template` WHERE `entry` = 68;

UPDATE `tmp_roy`
SET
    `entry` = 900001,
    `name` = 'Roy',
    `subname` = 'Services',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 129,
    `unit_flags` = 768,
    `ScriptName` = 'roy';

INSERT INTO `creature_template`
SELECT * FROM `tmp_roy`;

DROP TEMPORARY TABLE `tmp_roy`;

INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
(900001, 0, 33454, 0, 0, 0, 0),
(900001, 1, 35952, 0, 0, 0, 0);

INSERT INTO `creature` (`id1`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`) VALUES
(900001, 0, -8832.0, 628.0, 94.0, 0.7, 300, 0, 0),
(900001, 1, 1569.0, -4420.0, 16.0, 0.0, 300, 0, 0),
(900001, 571, 5804.0, 556.0, 651.0, 1.5, 300, 0, 0);
