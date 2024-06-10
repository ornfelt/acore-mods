UPDATE `creature_template` SET `lootid` = 12397, `ScriptName` = 'boss_highlord_kruul' WHERE `entry` = 18338;

DELETE FROM `creature` WHERE `id1` = 18338;
INSERT INTO `creature` (`guid`, `id1`, `id2`, `id3`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
('3500507','18338','0','0','0','0','0','1','1','1','-9143.28','379.317','90.7635','0.600393','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500508','18338','0','0','0','0','0','1','1','1','-5068.49','-791.5','495.127','5.73214','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500509','18338','0','0','1','0','0','1','1','1','1312.73','-4384.34','26.2469','0.299203','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500510','18338','0','0','0','0','0','1','1','1','1920.75','239.956','49.7667','3.17175','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500511','18338','0','0','0','0','0','1','1','1','196.027','-2090.45','116.538','5.62518','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500513','18338','0','0','0','0','0','1','1','1','1919.06','-2920.31','75.5114','4.26643','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500514','18338','0','0','1','0','0','1','1','1','6687.23','-4577.16','721.137','5.70474','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500515','18338','0','0','0','0','0','1','1','1','-14222.8','346.07','26.4181','3.91087','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500516','18338','0','0','1','0','0','1','1','1','2754.08','-3903.53','91.6572','1.13688','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500517','18338','0','0','1','0','0','1','1','1','-6917.24','905.308','25.0717','5.60217','21600','0','0','607970','0','0','0','0','0','',NULL),
('3500518','18338','0','0','0','0','0','1','1','1','-6483.52','-1105.19','302.162','4.46454','21600','0','0','607970','0','0','0','0','0','',NULL);

DELETE FROM `pool_template` WHERE `entry` = 18338;
INSERT INTO `pool_template` (`entry`, `max_limit`, `description`) VALUES
(18338, 1, "Black Portal Event - Highlord Kruul");

DELETE FROM `pool_creature` WHERE `pool_entry` = 15813;
INSERT INTO `pool_creature` (`guid`, `pool_entry`, `chance`, `description`) VALUES
(3500507, 15813, 0, 'Black Portal Event - Highlord Kruul - SW'),
(3500508, 15813, 0, 'Black Portal Event - Highlord Kruul - IF'),
(3500509, 15813, 0, 'Black Portal Event - Highlord Kruul - OG'),
(3500510, 15813, 0, 'Black Portal Event - Highlord Kruul - UC'),
(3500511, 15813, 0, 'Black Portal Event - Highlord Kruul - Hinterlands'),
(3500513, 15813, 0, 'Black Portal Event - Highlord Kruul - EP'),
(3500514, 15813, 0, 'Black Portal Event - Highlord Kruul - WS'),
(3500515, 15813, 0, 'Black Portal Event - Highlord Kruul - STV'),
(3500516, 15813, 0, 'Black Portal Event - Highlord Kruul - Azshara'),
(3500517, 15813, 0, 'Black Portal Event - Highlord Kruul - Silithus'),
(3500518, 15813, 0, 'Black Portal Event - Highlord Kruul - Searing Gorge');
