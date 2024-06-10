DELETE FROM `command` WHERE `name` = 'worldboss locks';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('worldboss locks', 1, 'Syntax: worldboss locks \nDisplays worldboss locks for the current character and their expiration dates.');
