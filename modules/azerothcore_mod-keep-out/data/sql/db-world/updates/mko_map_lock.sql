CREATE TABLE IF NOT EXISTS `mod_mko_map_lock` (
  `mapId` smallint(6) UNSIGNED DEFAULT NULL,
  `zoneID` smallint(6) UNSIGNED DEFAULT NULL,
  `comment` varchar(255) DEFAULT '',
  CONSTRAINT `MKO_Map` UNIQUE (`mapId`, `zoneID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
