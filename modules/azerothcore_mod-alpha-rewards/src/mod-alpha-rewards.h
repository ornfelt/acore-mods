#ifndef ALPHAREWARDS_H
#define ALPHAREWARDS_H

#include "ScriptMgr.h"
#include "Language.h"
#include "Player.h"
#include "Vehicle.h"
#include "ObjectAccessor.h"
#include "Group.h"
#include "Map.h"
#include "Log.h"
#include "Opcodes.h"
#include <vector>
#include <algorithm>
#include "BattlegroundMgr.h"
#include "AccountMgr.h"
#include <map>
#include <unordered_map>
#include "Configuration/Config.h"

class AlphaRewards
{
public:
    void OnLoginCheck(Player* player);
    void OnLogoutUpdate(Player* player);
    void AddGamePoint(Player* player, uint32 game_point);
    void LoadAlphaRewardsTable();
    void SetInitialWorldSettings();
    std::unordered_map<uint32, uint32> AlphaQuestPointsMap;
    std::unordered_map<uint32, uint32> AlphaCreaturePointsMap;
    std::unordered_map<uint32, uint32> AlphaAchievementPointsMap;
    bool enabled;
};

class AlphaRewardData : public DataMap::Base
{
public:
    AlphaRewardData() {}
    AlphaRewardData(uint32 RewardPointsMap) : RewardPointsMap(RewardPointsMap) { reset(); }
    uint32 RewardPointsMap = 0;
    uint32 timer = sConfigMgr->GetIntDefault("RewardPlayedTime", 2)*HOUR*IN_MILLISECONDS; // make this config Driven and rmoev below void Reset();

    void reset()
    {
        timer = static_cast <uint32>(sConfigMgr->GetIntDefault("RewardPlayedTime", 2))* HOUR*IN_MILLISECONDS;
    }
};

#define sAlphaRewards ACE_Singleton<AlphaRewards, ACE_Null_Mutex>::instance()

#endif
