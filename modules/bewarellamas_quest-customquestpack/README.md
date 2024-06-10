# quest-customquestpack
Custom Quests for AzerothCore private servers.

The goal of this is to add a number of custom quests that provide unobtainable items on AzerothCore servers. The intent is to have this be portable and not require manipulation of DBC file, client side patches, modification to existing database entries, or any other file/databse manipulation beyond running basic SQL queries provided.

Check the QuestInfo.md file for information on specific quests.


## How to Install
1. Place in the 'modules' folder of your AzerothCore source folder.
2. Import SQL into the appropriate database (World db)
3. Rerun CMake and launch a clean build of AzerothCore.
4. (Optional) Copy customquestpack.conf.dist to customquestpack.conf. Edit the configuration file to enable or disable quest triggers.

FYI: The config will only disable quest triggers that happen in the code. Most spawns, quests and dialogues are handled by SQL now. IF you want to disable certain quests you will need to remove the appropriate QL entries.

### Credits
* [Bewarellamas (me)](https://github.com/bewarellamas): Quest design, coding, and testing
* [AzerothCore module creators](https://www.azerothcore.org/catalogue.html): Example code and inspiration
* [Wow.tools](https://wow.tools): DBC information
* [Wowhead.com](https://wowhead.com): Information and IDs in many areas
