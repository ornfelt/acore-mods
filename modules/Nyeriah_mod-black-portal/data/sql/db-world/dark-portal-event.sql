--
UPDATE `creature_template` SET `ScriptName` = 'npc_marshal_raynor' WHERE `entry` = 19385;

DELETE FROM `creature_template` WHERE `entry` = 192880;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `speed_swim`, `speed_flight`, `detection_range`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
('192880','0','0','0','0','0','10691','0','0','0','Dovanath','Herald of the Burning Legion',NULL,'0','63','63','1','1754','0','1','1.14286','1','1','20','1','1','0','7.5','2000','2000','1','1','2','0','2048','0','0','0','0','0','0','3','0','0','0','0','0','0','0','0','','0','1','20','5','1','1','0','0','1','0','0','0','npc_dark_portal_boss','0');
UPDATE `creature_template` SET `scale` = 2, HealthModifier = 600, `mechanic_immune_mask` = 616775551, `type_flags` = 4, `DamageModifier` = 35 WHERE `entry` = 192880;
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` |33554432 WHERE `entry` = 192880;

DELETE FROM `creature_text` WHERE `CreatureID` = 192880;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(192880, 0, 0, 'I herald the end of this world, mortals!', 14, 0, 100, 0, 0, 0, 0, 4, 'Dovanath - On Spawn'),
(192880, 1, 0, 'Your soul is mine!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Enter Combat'),
(192880, 1, 1, 'Death rages!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Enter Combat'),
(192880, 1, 2, 'Die!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Enter Combat'),
(192880, 2, 0, 'Dovanath calls down an Infernal Storm!', 41, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Storm - 30% hp'),
(192880, 3, 0, 'Despair as your allies perish. Soon you will join them in death!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Just Killed'),
(192880, 3, 1, 'Your resistance proves fragile. Is this all this world has to offer?', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Just Killed'),
(192880, 3, 2, 'None shall oppose the Burning Legion\'s plans!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Just Killed'),
(192880, 3, 3, 'Failure, as expected!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Just Killed'),
(192880, 4, 0, 'ENOUGH! I grow tired of those petty tricks. It\'s time you witness the might of the Burning Legion!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Just Killed'),
(192880, 5, 0, 'UNTHINKABLE! Pathetic mortals! My demise accomplishes nothing! The Burning Legion will engulf your world in fel flames!', 14, 0, 100, 0, 0, 0, 0, 0, 'Dovanath - Just Died');

DELETE FROM `creature` WHERE `guid` IN (3800534, 3800522, 3800521, 3800523);
INSERT INTO `creature` (`guid`, `id1`, `id2`, `id3`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
('3800534','18966','0','0','0','0','0','1','1','1','-11842.9','-3210.99','-29.4737','2.55352','300','0','0','111780','47325','0','0','0','0','',NULL),
('3800522','18969','0','0','0','0','0','1','1','1','-11839.8','-3189.39','-29.4783','4.62059','300','0','0','111780','47325','0','0','0','0','',NULL),
('3800521','19385','0','0','0','0','0','1','1','1','-11839.5','-3201.4','-29.8483','0.133875','300','0','0','69860','0','0','0','0','0','',NULL),
('3800523','19942','0','0','0','0','0','1','1','1','-11839.4','-3218.15','-29.7824','1.98715','300','0','0','5228','0','0','0','0','0','',NULL);

UPDATE `creature_template` SET `flags_extra` = `flags_extra` |2097152 WHERE `entry` = 19284;
