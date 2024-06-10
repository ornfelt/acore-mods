DELETE FROM `creature_template` WHERE `entry` IN (9000000, 9000001, 9000002);

INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `speed_swim`, `speed_flight`, `detection_range`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(9000000, 0, 0, 0, 0, 0, 22879, 0, 0, 0, '"Captain" Napoleon', 'Bots for hire', 'Speak', 0, 80, 80, 0, 120, 0, 1, 1.14286, 1, 1, 12, 1, 4, 0, 20, 2000, 2000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 100, 1, 20, 1, 0, 0, 1, 0, 0, 0, 'bot_giver', 12340),
(9000001, 0, 0, 0, 0, 0, 26582, 0, 0, 0, 'Mal\'Ganis', '', 'Speak', 0, 80, 80, 2, 35, 1, 1, 1.14286, 1, 1, 12, 0.35, 0, 5, 1, 1300, 2000, 14, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 7, 4168, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1.75, 2, 10, 1, 0, 0, 1, 0, 32, 0, 'bot_dreadlord', 12340),
(9000002, 0, 0, 0, 0, 0, 169, 0, 0, 0, 'Infernal', '', '', 0, 80, 80, 2, 35, 0, 1.2, 1.3, 1, 1, 10, 1.5, 0, 2, 2, 2000, 2000, 1, 1, 1, 0, 0, 0, 19, 0, 0, 0, 0, 3, 4104, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 8, 1, 1, 1, 0, 121, 1, 0, 0, 0, 'bot_infernal', 12340);