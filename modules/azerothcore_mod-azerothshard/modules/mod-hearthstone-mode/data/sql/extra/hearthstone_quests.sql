DROP TABLE IF EXISTS `hearthstone_quests`;
CREATE TABLE `hearthstone_quests`(  
  `id` BIGINT(10) NOT NULL,
  `flag` INT(10) DEFAULT 0, -- 1 pve, 2 pvp
  PRIMARY KEY (`id`)
);

-- pve quests
DELETE FROM hearthstone_quests WHERE id >= 100000 AND id <= 100080;
INSERT INTO hearthstone_quests (id) VALUES
(100000),(100001),(100002),(100003),(100004),(100005),(100006),(100007),(100008),(100009),(100010),(100011),(100012),(100013),(100014),(100015),(100016),(100017),(100018),(100019),(100020),(100021),(100022),(100023),(100024),(100025),(100026),(100027),(100028),(100029),(100030),(100031),(100032),(100033),(100034),(100035),(100036),(100037),(100038),(100039),(100040),(100041),(100042),(100043),(100044),(100045),(100046),(100047),(100048),(100049),(100050),(100051),(100052),(100053),(100054),(100055),(100056),(100057),(100058),(100059),(100060),(100061),(100062),(100063),(100064),(100065),(100066),(100067),(100068),(100069),(100070),(100071),(100072),(100073),(100074),(100075),(100076),(100077),(100078),(100079),(100080);

UPDATE hearthstone_quests SET flag = 1 WHERE id >= 100000 AND id <= 100080; 

-- pvp quests
DELETE FROM hearthstone_quests WHERE id >= 110040 AND id <= 110095;
INSERT INTO hearthstone_quests (id) VALUES
(110040),(110041),(110042),(110043),(110044),(110045),(110046),(110047),(110048),(110049),(110050),(110051),(110052),(110053),(110054),(110055),(110056),(110057),(110058),(110059),(110060),(110061),(110062),(110063),(110064),(110065),(110066),(110067),(110068),(110069),(110070),(110071),(110072),(110073),(110074),(110075),(110076),(110077),(110078),(110079),(110080),(110081),(110082),(110083),(110084),(110085),(110086),(110087),(110088),(110089),(110090),(110091),(110092),(110093),(110094),(110095);

UPDATE hearthstone_quests SET flag = 2 WHERE id >= 110040 AND id <= 110095;