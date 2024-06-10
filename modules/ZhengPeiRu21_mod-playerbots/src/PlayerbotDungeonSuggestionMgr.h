/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_PLAYERBOTDUNGEONSUGGESTIONMGR_H
#define _PLAYERBOT_PLAYERBOTDUNGEONSUGGESTIONMGR_H

#include "Common.h"
#include "DBCEnums.h"

#include <map>
#include <vector>

struct DungeonSuggestion
{
    std::string name;
    Difficulty difficulty;
    uint8 min_level;
    uint8 max_level;
    std::string abbrevation;
    std::string strategy;
};

class PlayerbotDungeonSuggestionMgr
{
    public:
        PlayerbotDungeonSuggestionMgr() { };
         ~PlayerbotDungeonSuggestionMgr() { };
        static PlayerbotDungeonSuggestionMgr* instance()
        {
            static PlayerbotDungeonSuggestionMgr instance;
            return &instance;
        }

        void LoadDungeonSuggestions();
        std::vector<DungeonSuggestion> const GetDungeonSuggestions();

    private:
        std::vector<DungeonSuggestion> m_dungeonSuggestions;
};

#define sPlayerbotDungeonSuggestionMgr PlayerbotDungeonSuggestionMgr::instance()

#endif
