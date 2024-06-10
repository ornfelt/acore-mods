SET @ENTRY:=40000;
DELETE FROM `acore_string` WHERE `entry`=@ENTRY;
INSERT INTO `acore_string` (`entry`, `content_default`) VALUES
(@ENTRY, 'Reloaded morph_all_command');
