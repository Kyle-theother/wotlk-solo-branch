-- Equipment proficiency trainer, entry 600001.
-- Cloned from Jeeves so it matches the current creature_template columns.

DELETE FROM `creature_template_model` WHERE `CreatureID` = 600001;
DELETE FROM `creature_template` WHERE `entry` = 600001;
DELETE FROM `npc_text` WHERE `ID` = 600001;

CREATE TEMPORARY TABLE `tmp_goliath` AS SELECT * FROM `creature_template` WHERE `entry` = 35642;
UPDATE `tmp_goliath` SET
    `entry` = 600001,
    `name` = 'The Gentle Goliath',
    `subname` = 'Equipment Proficiency Trainer',
    `IconName` = 'Speak',
    `gossip_menu_id` = 0,
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `unit_flags` = 2,
    `type` = 7,
    `flags_extra` = 2,
    `AiName` = 'SmartAI',
    `ScriptName` = 'SubClass_NPC';
INSERT INTO `creature_template` SELECT * FROM `tmp_goliath`;
DROP TEMPORARY TABLE `tmp_goliath`;

INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`) VALUES
(600001, 0, 26571, 1, 1);

INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(600001, 'Greetings $N. I can teach you anything and everything about equipment!!!');
