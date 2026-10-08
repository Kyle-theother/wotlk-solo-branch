-- The Gentle Goliath, entry 600001, beside the class trainers in each starting zone.
-- Offsets are a few yards from the Jarvis starting-zone spawns so the two NPCs do not stack.

DELETE FROM `creature` WHERE `id` = 600001;

INSERT INTO `creature` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`) VALUES
(600001, 0, -8942.0, -129.0, 83.5, 3.2, 300, 0, 0),
(600001, 0, -6231.0, 334.0, 383.0, 2.4, 300, 0, 0),
(600001, 0, 1673.7, 1688.9, 121.7, 0.9, 300, 0, 0),
(600001, 1, -610.0, -4248.0, 38.7, 2.1, 300, 0, 0),
(600001, 1, -2909.0, -254.0, 53.0, 2.2, 300, 0, 0),
(600001, 1, 10318.0, 835.0, 1326.4, 2.4, 300, 0, 0),
(600001, 530, -3953.0, -13928.0, 100.6, 5.3, 300, 0, 0),
(600001, 530, 10356.0, -6354.0, 33.4, 2.2, 300, 0, 0),
(600001, 609, 2361.0, -5661.0, 426.0, 0.5, 300, 0, 0);
