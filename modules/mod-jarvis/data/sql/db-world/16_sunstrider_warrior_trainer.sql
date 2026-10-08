-- Warrior trainer on Sunstrider Isle, beside the other class trainers.
-- Entry 2119 is Dannal Stern, an existing warrior trainer, so the spells come with the template.

DELETE FROM `creature` WHERE `id` = 2119 AND `map` = 530 AND `position_x` BETWEEN 10340 AND 10360;

INSERT INTO `creature` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`) VALUES
(2119, 530, 10348.0, -6362.0, 33.4, 1.6, 300, 0, 0);
