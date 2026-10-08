-- Jarvis beside each race's first spawn. This database uses creature.id, not id1.
-- Delete the previous start-zone rows first so a changed file does not leave duplicates.

DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 0 AND `position_x` BETWEEN -8960 AND -8930 AND `position_y` BETWEEN -150 AND -110;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 0 AND `position_x` BETWEEN -6250 AND -6220 AND `position_y` BETWEEN 310 AND 350;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 0 AND `position_x` BETWEEN 1655 AND 1690 AND `position_y` BETWEEN 1675 AND 1710;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 1 AND `position_x` BETWEEN -630 AND -590 AND `position_y` BETWEEN -4270 AND -4230;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 1 AND `position_x` BETWEEN -2930 AND -2890 AND `position_y` BETWEEN -275 AND -235;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 1 AND `position_x` BETWEEN 10300 AND 10330 AND `position_y` BETWEEN 810 AND 850;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 530 AND `position_x` BETWEEN -3975 AND -3940 AND `position_y` BETWEEN -13950 AND -13910;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 530 AND `position_x` BETWEEN 10320 AND 10380 AND `position_y` BETWEEN -6390 AND -6320;
DELETE FROM `creature` WHERE `id` = 900010 AND `map` = 609 AND `position_x` BETWEEN 2340 AND 2375 AND `position_y` BETWEEN -5680 AND -5645;

INSERT INTO `creature` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`) VALUES
(900010, 0, -8945.0, -132.0, 83.5, 0.0, 300, 0, 0),
(900010, 0, -6235.0, 331.0, 383.0, 0.8, 300, 0, 0),
(900010, 0, 1670.7, 1691.9, 121.7, 2.5, 300, 0, 0),
(900010, 1, -614.0, -4251.0, 38.7, 0.5, 300, 0, 0),
(900010, 1, -2913.0, -257.0, 53.0, 4.7, 300, 0, 0),
(900010, 1, 10315.0, 832.0, 1326.4, 5.5, 300, 0, 0),
(900010, 530, -3957.0, -13931.0, 100.6, 2.2, 300, 0, 0),
(900010, 530, 10353.0, -6357.0, 33.4, 5.3, 300, 0, 0),
(900010, 609, 2358.0, -5664.0, 426.0, 3.6, 300, 0, 0);
