-- Orgrimmar Jarvis was clipped into the Valley of Strength. Move it into the open and keep it selectable.
UPDATE `creature_template`
SET `type` = 7,
    `npcflag` = 129,
    `unit_flags` = 768,
    `unit_flags2` = 0,
    `dynamicflags` = 0,
    `flags_extra` = 2,
    `gossip_menu_id` = 0
WHERE `entry` = 900010;

UPDATE `creature`
SET `position_x` = 1576.4,
    `position_y` = -4437.8,
    `position_z` = 16.6,
    `orientation` = 1.6,
    `wander_distance` = 0,
    `MovementType` = 0
WHERE `id` = 900010 AND `map` = 1 AND `position_x` BETWEEN 1500 AND 1700 AND `position_y` BETWEEN -4500 AND -4300;
