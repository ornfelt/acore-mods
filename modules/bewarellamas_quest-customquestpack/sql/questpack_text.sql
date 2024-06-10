
SET @TEXT_ID = 197761,
@TEXT_ID2 = 197762,
@TEXT0= 'Duridz help the critterz. Don\'t like when critterz get the deadz lulz.',
@TEXT1= 'Iz found a stick! I use to donk scaries when not a kitty duridz lul.',
@TEXT2= 'Sumtimez other duridz make me poop in a box so the floor don\'t get messy.',
@TEXT1A= 'Durid is no need homez lol, but has littel gryffie that need home. Here go!';


DELETE FROM `npc_text` WHERE `ID` IN (@TEXT_ID);
DELETE FROM `npc_text` WHERE `ID` IN (@TEXT_ID2);

INSERT INTO `npc_text` (`ID`, `text0_1`, `Probability0`, `text1_1`, `Probability1`, `text2_1`, `Probability2`) VALUES (@TEXT_ID, @TEXT0, 50, @TEXT1, 45, @TEXT2, 5);

INSERT INTO `npc_text` (`ID`, `text0_1`, `Probability0`) VALUES (@TEXT_ID2, @TEXT1A, 100);


SET @TEXT_ID2_0 = 197763,
@TEXT2_0 = 'Oh man... There is no way this is up to code...';

DELETE FROM `npc_text` WHERE `ID` IN (@TEXT_ID2_0);
INSERT INTO `npc_text` (`ID`, `text0_0`, `Probability0`) VALUES (@TEXT_ID2_0, @TEXT2_0, 100);