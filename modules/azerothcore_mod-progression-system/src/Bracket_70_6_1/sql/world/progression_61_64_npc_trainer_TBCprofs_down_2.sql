-- Restore Brilliant Glass for Sunwell
DELETE FROM `npc_trainer` WHERE `SpellID`=47280;
INSERT INTO `npc_trainer` (`ID`, `SpellID`, `MoneyCost`, `ReqSkillLine`, `ReqSkillRank`, `ReqLevel`, `ReqSpell`) VALUES
(201025, 47280, 30000, 755, 350, 0, 0);
