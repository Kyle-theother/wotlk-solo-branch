-- Jarvis. Entry 900010. Model copied from Jeeves (35642).
-- This database uses creature.id, not id1.

DELETE FROM `creature` WHERE `id` = 900010;
DELETE FROM `creature_template_model` WHERE `CreatureID` = 900010;
DELETE FROM `creature_template` WHERE `entry` = 900010;

CREATE TEMPORARY TABLE `tmp_jarvis` AS
SELECT * FROM `creature_template` WHERE `entry` = 35642;

UPDATE `tmp_jarvis`
SET
    `entry` = 900010,
    `name` = 'Jarvis',
    `subname` = 'Services',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `unit_flags` = 768,
    `ScriptName` = 'npc_jarvis';

INSERT INTO `creature_template`
SELECT * FROM `tmp_jarvis`;

DROP TEMPORARY TABLE `tmp_jarvis`;

INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`)
SELECT 900010, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`
FROM `creature_template_model`
WHERE `CreatureID` = 35642;

INSERT INTO `creature` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`) VALUES
(900010, 0, -8832.0, 628.0, 94.0, 0.7, 300, 0, 0),
(900010, 0, -4920.0, -946.0, 502.0, 5.4, 300, 0, 0),
(900010, 0, 1632.0, 240.0, -43.0, 6.2, 300, 0, 0),
(900010, 1, 1569.0, -4420.0, 16.0, 0.0, 300, 0, 0),
(900010, 1, -1277.0, 124.0, 131.0, 4.6, 300, 0, 0),
(900010, 1, 9866.0, 2494.0, 1316.0, 5.5, 300, 0, 0),
(900010, 530, -3965.0, -11653.0, -138.0, 5.6, 300, 0, 0),
(900010, 530, 9470.0, -7278.0, 14.0, 6.1, 300, 0, 0),
(900010, 530, -1833.0, 5300.0, -12.0, 2.0, 300, 0, 0),
(900010, 571, 5804.0, 556.0, 651.0, 1.5, 300, 0, 0);
