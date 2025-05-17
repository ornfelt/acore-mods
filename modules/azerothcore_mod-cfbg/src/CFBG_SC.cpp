/*
 *  Copyright (С) since 2019 Andrei Guluaev (Winfidonarleyan/Kargatum) https://github.com/Winfidonarleyan
 *  Copyright (С) since 2019+ AzerothCore <www.azerothcore.org>
 */

#include "CFBG.h"
#include "Group.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "BattlegroundQueue.h"

// CFBG custom script
class CFBG_BG : public BGScript
{
public:
    CFBG_BG() : BGScript("CFBG_BG", {
        ALLBATTLEGROUNDHOOK_ON_BATTLEGROUND_BEFORE_ADD_PLAYER,
        ALLBATTLEGROUNDHOOK_ON_BATTLEGROUND_ADD_PLAYER,
        ALLBATTLEGROUNDHOOK_ON_BATTLEGROUND_END_REWARD,
        ALLBATTLEGROUNDHOOK_ON_BATTLEGROUND_REMOVE_PLAYER_AT_LEAVE,
        ALLBATTLEGROUNDHOOK_ON_ADD_GROUP,
        ALLBATTLEGROUNDHOOK_CAN_FILL_PLAYERS_TO_BG,
        ALLBATTLEGROUNDHOOK_IS_CHECK_NORMAL_MATCH,
        ALLBATTLEGROUNDHOOK_CAN_SEND_MESSAGE_BG_QUEUE
    }) {}

    void OnBattlegroundBeforeAddPlayer(Battleground* bg, Player* player) override
    {
        sCFBG->ValidatePlayerForBG(bg, player);
    }

    void OnBattlegroundAddPlayer(Battleground* bg, Player* player) override
    {
        sCFBG->FitPlayerInTeam(player, true, bg);

        if (sCFBG->IsEnableResetCooldowns())
            player->RemoveArenaSpellCooldowns(true);
    }

    void OnBattlegroundEndReward(Battleground* bg, Player* player, TeamId /*winnerTeamId*/) override
    {
        if (!sCFBG->IsEnableSystem() || !bg || !player || bg->isArena())
            return;

        if (sCFBG->IsPlayerFake(player))
            sCFBG->ClearFakePlayer(player);
    }

    void OnBattlegroundRemovePlayerAtLeave(Battleground* bg, Player* player) override
    {
        if (!sCFBG->IsEnableSystem() || bg->isArena())
            return;

        sCFBG->FitPlayerInTeam(player, false, bg);

        if (sCFBG->IsPlayerFake(player))
            sCFBG->ClearFakePlayer(player);
    }

    void OnAddGroup(BattlegroundQueue* queue, GroupQueueInfo* ginfo, uint32& index, Player* /*leader*/, Group* /*group*/, BattlegroundTypeId /* bgTypeId */, PvPDifficultyEntry const* /* bracketEntry */,
        uint8 /* arenaType */, bool /* isRated */, bool /* isPremade */, uint32 /* arenaRating */, uint32 /* matchmakerRating */, uint32 /* arenaTeamId */, uint32 /* opponentsArenaTeamId */) override
    {
        if (!queue)
            return;

        if (sCFBG->IsEnableSystem() && !ginfo->ArenaType && !ginfo->IsRated)
            index = BG_QUEUE_CFBG;

        // After rework hook
        // sCFBG->OnAddGroupToBGQueue(ginfo, group);
    }

    bool CanFillPlayersToBG(BattlegroundQueue* queue, Battleground* bg, BattlegroundBracketId bracket_id) override
    {
        return !sCFBG->FillPlayersToCFBG(queue, bg, bracket_id);
    }

    bool IsCheckNormalMatch(BattlegroundQueue* queue, Battleground* bg, BattlegroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers) override
    {
        if (!sCFBG->IsEnableSystem() || bg->isArena())
            return false;

        return sCFBG->CheckCrossFactionMatch(queue, bracket_id, minPlayers, maxPlayers);
    }

    bool CanSendMessageBGQueue(BattlegroundQueue* queue, Player* leader, Battleground* bg, PvPDifficultyEntry const* bracketEntry) override
    {
        if (bg->isArena() || !sCFBG->IsEnableSystem())
        {
            // if it's arena OR the CFBG is disabled, let the core handle the announcement
            return true;
        }

        // otherwise, let the CFBG module handle the announcement
        sCFBG->SendMessageQueue(queue, bg, bracketEntry, leader);
        return false;
    }
};

class CFBG_Player : public PlayerScript
{
public:
    CFBG_Player() : PlayerScript("CFBG_Player", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_CAN_JOIN_IN_BATTLEGROUND_QUEUE,
        PLAYERHOOK_ON_BEFORE_UPDATE,
        PLAYERHOOK_ON_BEFORE_SEND_CHAT_MESSAGE,
        PLAYERHOOK_ON_REPUTATION_CHANGE
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (!sCFBG->IsEnableSystem())
            return;

        if (player->GetTeamId(true) != player->GetBgTeamId())
            sCFBG->FitPlayerInTeam(player, player->GetBattleground() && !player->GetBattleground()->isArena(), player->GetBattleground());
    }

    bool OnPlayerCanJoinInBattlegroundQueue(Player* player, ObjectGuid /*BattlemasterGuid*/ , BattlegroundTypeId /*BGTypeID*/, uint8 joinAsGroup, GroupJoinBattlegroundResult& err) override
    {
        if (!sCFBG->IsEnableSystem())
            return true;

        if (joinAsGroup)
        {
            Group* group = player->GetGroup();
            if (!group)
                return true;

            if (group->isRaidGroup() || group->GetMembersCount() > sCFBG->GetMaxPlayersCountInGroup())
                err = ERR_BATTLEGROUND_JOIN_FAILED;

            return false;
        }

        return true;
    }

    void OnPlayerBeforeUpdate(Player* player, uint32 diff) override
    {
        if (timeCheck <= diff)
        {
            sCFBG->UpdateForget(player);
            timeCheck = 10000;
        }
        else
            timeCheck -= diff;
    }

    void OnPlayerBeforeSendChatMessage(Player* player, uint32& type, uint32& lang, std::string& /*msg*/) override
    {
        if (!player || !sCFBG->IsEnableSystem())
            return;

        Battleground* bg = player->GetBattleground();

        if (!bg || bg->isArena())
            return;

        // skip addon lang and universal
        if (lang == LANG_UNIVERSAL || lang == LANG_ADDON)
            return;

        // skip addon and system message
        if (type == CHAT_MSG_ADDON || type == CHAT_MSG_SYSTEM)
            return;

        // to gm lang
        lang = LANG_UNIVERSAL;
    }

    bool OnPlayerReputationChange(Player* player, uint32 factionID, int32& standing, bool /*incremental*/) override
    {
        uint32 repGain = player->GetReputation(factionID);
        TeamId teamId = player->GetTeamId(true);

        if ((factionID == FACTION_FROSTWOLF_CLAN && teamId == TEAM_ALLIANCE) ||
            (factionID == FACTION_STORMPIKE_GUARD && teamId == TEAM_HORDE))
        {
            uint32 diff = standing - repGain;
            player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(teamId == TEAM_ALLIANCE ? FACTION_STORMPIKE_GUARD : FACTION_FROSTWOLF_CLAN), diff);
            return false;
        }

        return true;
    }

private:
    uint32 timeCheck = 10000;
};

class CFBG_World : public WorldScript
{
public:
    CFBG_World() : WorldScript("CFBG_World", {
        WORLDHOOK_ON_AFTER_CONFIG_LOAD
    }) { }

    void OnAfterConfigLoad(bool /*Reload*/) override
    {
        sCFBG->LoadConfig();
    }
};

void AddSC_CFBG()
{
    new CFBG_BG();
    new CFBG_Player();
    new CFBG_World();
}
