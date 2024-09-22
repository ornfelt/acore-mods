DELETE FROM `creature_template` WHERE `entry`=999991;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(999991, 0, 0, 0, 0, 0, 'Arena Battlemaster 1v1', '', '', 8218, 70, 70, 2, 35, 1048577, 1.1, 1.14286, 1, 0, 0, 1, 2000, 2000, 1, 768, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 'npc_1v1arena', 12340);

DELETE FROM `creature_template_model`  WHERE `CreatureID` In (999991);
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(999991, 0, 7110, 1, 1, 23766);

DELETE FROM `battlemaster_entry` WHERE `entry`=999991;
INSERT INTO `battlemaster_entry` (`entry`, `bg_template`) VALUES (999991, 6);

-- Command
DELETE FROM `command` WHERE `name` IN ('q1v1', 'q1v1 rated', 'q1v1 unrated');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('q1v1', 0, 'Syntax .q1v1 rated/unrate\nJoin arena 1v1 rated or unrated'),
('q1v1 rated', 0, 'Syntax .q1v1 rated\nJoin arena 1v1 rated'),
('q1v1 unrated', 0, 'Syntax .q1v1 unrated\nJoin arena 1v1 unrated');
