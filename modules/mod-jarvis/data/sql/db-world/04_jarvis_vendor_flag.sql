-- Gossip plus vendor, so the sale option can open a shop window.
UPDATE `creature_template`
SET `npcflag` = 129, `ScriptName` = 'npc_jarvis'
WHERE `entry` = 900010;
