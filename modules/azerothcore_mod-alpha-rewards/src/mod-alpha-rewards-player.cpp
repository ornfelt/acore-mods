#include "ScriptMgr.h"
#include "Player.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "QuestDef.h"
#include "DataMap.h"
#include "mod-alpha-rewards.h"
#include "ace/ACE.h"

class LoadAlphaRewardGlobal : public WorldScript
{
public:
    LoadAlphaRewardGlobal() : WorldScript("LoadAlphaRewardGlobal") { }

    void OnLoadCustomDatabaseTable() override
    {
        sAlphaRewards->LoadAlphaRewardsTable();
    }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        sAlphaRewards->SetInitialWorldSettings();
    }
};

class AlphaRewardPlayer : public PlayerScript
{
public:
    AlphaRewardPlayer() : PlayerScript("AlphaRewardPlayer") { }

    //On Login Get Current GamePoints
    void OnLogin(Player* p) override
    {
        sAlphaRewards->OnLoginCheck(p);
    }

    //On logout update the gamepoints.
    void OnLogout(Player* p) override
    {
        sAlphaRewards->OnLogoutUpdate(p);
    }

    // Reward for Active Game Time staying logged in
    void OnBeforeUpdate(Player* player, uint32 p_time) override
    {
        if (player->isAFK())
            return;

        AlphaRewardData* RewardPoint = player->CustomData.GetDefault<AlphaRewardData>("RewardPoint");

        if (RewardPoint->timer <= p_time)
        {
            RewardPoint->reset();
            sAlphaRewards->AddGamePoint(player, 1);
        }
            
    }

    void OnPlayerCompleteQuest(Player* player, Quest const* quest) override
    {
        auto it = sAlphaRewards->AlphaQuestPointsMap.find(quest->GetQuestId());

        if (it != sAlphaRewards->AlphaQuestPointsMap.end())
            sAlphaRewards->AddGamePoint(player, it->second);

        // Do not remove below its there for testing purpose :)
        // ChatHandler(player->GetSession()).PSendSysMessage("You Currently have %u game points", player->CustomData.Get< AlphaRewardData>("RewardPointsMap")->RewardPointsMap);
    }

    void OnAchiComplete(Player* player, AchievementEntry const* achievement) override
    {
        auto it = sAlphaRewards->AlphaAchievementPointsMap.find(achievement->ID);

        if (it != sAlphaRewards->AlphaAchievementPointsMap.end())
            sAlphaRewards->AddGamePoint(player, it->second);
    }
};

class AlphaRewardGlobalScript : public GlobalScript
{
public:
    AlphaRewardGlobalScript() : GlobalScript("AlphaRewardGlobalScript") {}

    void OnAfterUpdateEncounterState(Map* map, EncounterCreditType /*type*/, uint32 /*creditEntry*/, Unit* source, Difficulty /*difficulty_fixed*/, DungeonEncounterList const* /*encounters*/, uint32 /*dungeonCompleted*/, bool /*updated*/) override
    {
        if (!map->IsDungeon() || !map->IsHeroic() || map->IsBattlegroundOrArena())
            return;

        Map::PlayerList const &playerList = map->GetPlayers();

        if (playerList.isEmpty())
            return;

        auto it = sAlphaRewards->AlphaCreaturePointsMap.find(source->GetEntry());

        if (it != sAlphaRewards->AlphaAchievementPointsMap.end())
        {
            for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
            {
                if (!itr->GetSource() || itr->GetSource()->IsGameMaster())
                    continue;

                sAlphaRewards->AddGamePoint(itr->GetSource(), it->second);
            }
        }

    }
};

void AddSC_AlphaRewardScript()
{
    new AlphaRewardPlayer();
    new LoadAlphaRewardGlobal();
    new AlphaRewardGlobalScript();
}
