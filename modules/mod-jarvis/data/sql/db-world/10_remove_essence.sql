-- Remove Essence of Immortals from Jarvis and the item table.
DELETE FROM `npc_vendor` WHERE `item` = 900014;
DELETE FROM `item_template` WHERE `entry` = 900014;
