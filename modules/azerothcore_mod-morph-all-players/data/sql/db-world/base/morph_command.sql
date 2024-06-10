SET @REQUIRED_GMLEVEL=3;
DELETE FROM `command` WHERE `name` IN ('morphall', 'demorphall');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('morphall', @REQUIRED_GMLEVEL, 'Syntax: .morphall #displayid \r\n\r\nChange players model id to #displayid.'),
('demorphall', @REQUIRED_GMLEVEL, 'Syntax: .demorphall #displayid \r\n\r\nDemorph all players.');
