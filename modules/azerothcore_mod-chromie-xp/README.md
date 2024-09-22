# mode-chromie-xp

This module has been developed for [ChromieCraft](http://chromiecraft.com/) to allow players to use `.beta activate` and go further with their progression.

## SQL Utils

### Flag toggle

```sql
SET @LOCK_FLAG = 33554432;

-- Check which players have the XP locked
SELECT * FROM `characters` WHERE playerFlags & @LOCK_FLAG = @LOCK_FLAG;

-- Check which players of a certain level have the xp unlocked
SELECT * FROM `characters` WHERE level = 39 AND (playerFlags & @LOCK_FLAG != @LOCK_FLAG);

-- Lock character XP - NOTE: the character MUST be offline.
UPDATE `characters` SET `playerFlags` = `playerFlags` | @LOCK_FLAG WHERE `name` = 'Craft';

-- Unlock character XP - NOTE: the character MUST be offline.
UPDATE `characters` SET `playerFlags` = `playerFlags` & ~@LOCK_FLAG WHERE `name` = 'Craft';

```

### Add beta tester
```sql
SET @CHARACTER_NAME = 'Craft';
INSERT INTO `chromie_beta_testers` (`guid`, `isBetaTester`, `comment`) VALUES 
((SELECT `guid` FROM `characters` WHERE `name` = @CHARACTER_NAME), 1, @CHARACTER_NAME);
```
