DROP TABLE IF EXISTS `alphaquestpoints`;

CREATE TABLE IF NOT EXISTS `alpha_reward_system` (
  `Entry` int(11) NOT NULL AUTO_INCREMENT,
  `TypeId` int(11) NOT NULL DEFAULT '0' COMMENT '1 = creature 2= quest 3= achievement',
  `Points` int(11) NOT NULL DEFAULT '0',
  `Comment` varchar(255) NOT NULL,
  PRIMARY KEY (`Entry`)
) ENGINE=InnoDB AUTO_INCREMENT=24791 DEFAULT CHARSET=utf8;

/*!40000 ALTER TABLE `alpha_reward_system` DISABLE KEYS */;
INSERT INTO `alpha_reward_system` (`Entry`, `TypeId`, `Points`, `Comment`) VALUES
	(24788, 2, 3, 'Daily Heroic Random (1st)'),
	(24790, 2, 1, 'Daily Normal Random (1st)');

