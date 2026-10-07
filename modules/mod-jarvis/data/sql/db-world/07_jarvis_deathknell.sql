-- Move the Deathknell Jarvis from the old spot to Tirisfal map 29.9, 71.8.
-- That map point converts to world 1670.7, 1691.9, 121.7.

UPDATE `creature`
SET `position_x` = 1670.7, `position_y` = 1691.9, `position_z` = 121.7, `orientation` = 2.5
WHERE `id` = 900010 AND `map` = 0 AND `position_x` BETWEEN 1600 AND 1900 AND `position_y` BETWEEN 1500 AND 1800;
