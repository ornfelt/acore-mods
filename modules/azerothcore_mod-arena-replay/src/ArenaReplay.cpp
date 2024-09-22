//
// Created by romain-p on 17/10/2021.
//
#include "ArenaReplayDatabaseConnection.h"
#include "ArenaReplay_loader.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "CharacterDatabase.h"
#include "Chat.h"
#include "Opcodes.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include <unordered_map>
#include "Base32.h"
#include "Config.h"
#include "ArenaTeamMgr.h"
#include <iomanip>

std::vector<Opcodes> watchList =
{
        SMSG_NOTIFICATION,
        SMSG_AURA_UPDATE,
        SMSG_WORLD_STATE_UI_TIMER_UPDATE,
        SMSG_COMPRESSED_UPDATE_OBJECT,
        SMSG_AURA_UPDATE_ALL,
        SMSG_NAME_QUERY_RESPONSE,
        SMSG_DESTROY_OBJECT,
        MSG_MOVE_START_FORWARD,
        MSG_MOVE_SET_FACING,
        MSG_MOVE_HEARTBEAT,
        MSG_MOVE_JUMP,
        SMSG_MONSTER_MOVE,
        MSG_MOVE_FALL_LAND,
        SMSG_PERIODICAURALOG,
        SMSG_ARENA_UNIT_DESTROYED,
        MSG_MOVE_START_STRAFE_RIGHT,
        MSG_MOVE_STOP_STRAFE,
        MSG_MOVE_START_STRAFE_LEFT,
        MSG_MOVE_STOP,
        MSG_MOVE_START_BACKWARD,
        MSG_MOVE_START_TURN_LEFT,
        MSG_MOVE_STOP_TURN,
        MSG_MOVE_START_TURN_RIGHT,
        SMSG_SPELL_START,
        SMSG_SPELL_GO,
        CMSG_CAST_SPELL,
        CMSG_CANCEL_CAST,
        SMSG_CAST_FAILED,
        SMSG_SPELL_START,
        SMSG_SPELL_FAILURE,
        SMSG_SPELL_DELAYED,
        SMSG_PLAY_SPELL_IMPACT,
        SMSG_FORCE_RUN_SPEED_CHANGE,
        SMSG_ATTACKSTART,
        SMSG_POWER_UPDATE,
        SMSG_ATTACKERSTATEUPDATE,
        SMSG_SPELLDAMAGESHIELD,
        SMSG_SPELLHEALLOG,
        SMSG_SPELLENERGIZELOG,
        SMSG_SPELLNONMELEEDAMAGELOG,
        SMSG_ATTACKSTOP,
        SMSG_EMOTE,
        SMSG_AI_REACTION,
        SMSG_PET_NAME_QUERY_RESPONSE,
        SMSG_CANCEL_AUTO_REPEAT,
        SMSG_UPDATE_OBJECT,
        SMSG_FORCE_FLIGHT_SPEED_CHANGE,
        SMSG_GAMEOBJECT_QUERY_RESPONSE,
        SMSG_FORCE_SWIM_SPEED_CHANGE,
        SMSG_GAMEOBJECT_DESPAWN_ANIM,
        SMSG_CANCEL_COMBAT,
        SMSG_DISMOUNTRESULT,
        SMSG_MOUNTRESULT,
        SMSG_DISMOUNT,
        CMSG_MOUNTSPECIAL_ANIM,
        SMSG_MOUNTSPECIAL_ANIM,
        SMSG_MIRRORIMAGE_DATA,
        CMSG_MESSAGECHAT,
        SMSG_MESSAGECHAT
};

/*
CMSG_CANCEL_MOUNT_AURA,
CMSG_ALTER_APPEARANCE
SMSG_SUMMON_CANCEL
SMSG_PLAY_SOUND
SMSG_PLAY_SPELL_VISUAL
CMSG_ATTACKSWING
CMSG_ATTACKSTOP*/

struct PacketRecord { uint32 timestamp; WorldPacket packet; };
struct MatchRecord { BattlegroundTypeId typeId; uint8 arenaTypeId; uint32 mapId; std::deque<PacketRecord> packets; };
std::unordered_map<uint32, MatchRecord> records;
std::unordered_map<uint64, MatchRecord> loadedReplays;
std::unordered_map<uint32, uint32> bgReplayIds;

class ArenaReplayServerScript : public ServerScript
{
public:
    ArenaReplayServerScript() : ServerScript("ArenaReplayServerScript") {}

    bool CanPacketSend(WorldSession* session, WorldPacket& packet) override
    {
        if (session == nullptr || session->GetPlayer() == nullptr)
            return true;

        Battleground* bg = session->GetPlayer()->GetBattleground();

        if (!bg)
            return true;

        const bool isReplay = bgReplayIds.find(bg->GetInstanceID()) != bgReplayIds.end();

        // ignore packet when no bg or casual games
        if (isReplay)
          return true;

        // ignore packets until arena started
        if (bg->GetStatus() != BattlegroundStatus::STATUS_IN_PROGRESS)
            return true;

        // record packets from 1 player of each team
        // iterate just in case a player leaves and used as reference
        for (auto it : bg->GetPlayers())
        {
            if (it.second->GetBgTeamId() == session->GetPlayer()->GetBgTeamId())
            {
                if (it.second->GetGUID() != session->GetPlayer()->GetGUID())
                    return true;
                else
                    break;
            }
        }

        // ignore packets not in watch list
        if (std::find(watchList.begin(), watchList.end(), packet.GetOpcode()) == watchList.end())
            return true;

        if (records.find(bg->GetInstanceID()) == records.end())
            records[bg->GetInstanceID()].packets.clear();
        MatchRecord& record = records[bg->GetInstanceID()];

        uint32 timestamp = bg->GetStartTime();
        record.typeId = bg->GetBgTypeID();
        record.arenaTypeId = bg->GetArenaType();
        record.mapId = bg->GetMapId();
        // push back packet inside queue of matchId 0
        record.packets.push_back({ timestamp, /* copy */ WorldPacket(packet) });
        return true;
    }
};

class ArenaReplayArenaScript : public ArenaScript {
public:
  ArenaReplayArenaScript() : ArenaScript("ArenaReplayArenaScript") {}

  bool OnBeforeArenaCheckWinConditions(Battleground *const bg) override {
    const bool isReplay = bgReplayIds.find(bg->GetInstanceID()) != bgReplayIds.end();

    // if isReplay then return false to exit from check condition
    return !isReplay;
  }
};

class ArenaReplayBGScript : public BGScript
{
public:
    ArenaReplayBGScript() : BGScript("ArenaReplayBGScript") {}

    void OnBattlegroundUpdate(Battleground* bg, uint32 /* diff */) override
    {
        const bool isReplay = bgReplayIds.find(bg->GetInstanceID()) != bgReplayIds.end();
        if (!isReplay)
            return;

        if (!bg->isArena() && !sConfigMgr->GetOption<bool>("ArenaReplay.SaveBattlegrounds", true))
        {
            return;
        }
        if (!bg->isRated() && !sConfigMgr->GetOption<bool>("ArenaReplay.SaveUnratedArenas", true))
        {
            return;
        }

        uint32 replayId = bgReplayIds.at(bg->GetInstanceID());

        int32 startDelayTime = bg->GetStartDelayTime();
        if (startDelayTime > 1000) // reduces StartTime only when watching Replay
        {
            bg->SetStartDelayTime(1000);
            bg->SetStartTime(bg->GetStartTime() + (startDelayTime - 1000));
        }

        if (bg->GetStatus() != BattlegroundStatus::STATUS_IN_PROGRESS)
            return;

        // retrieve arena replay data
        auto it = loadedReplays.find(replayId);
        if (it == loadedReplays.end())
            return;
        MatchRecord& match = it->second;

        // if replay ends or spectator left > free arena replay data and/or kick player
        if (match.packets.empty() || bg->GetPlayers().empty())
        {
            loadedReplays.erase(it);

            if (!bg->GetPlayers().empty())
                bg->GetPlayers().begin()->second->LeaveBattleground(bg);
            return;
        }

        //send replay data to spectator
        while (!match.packets.empty() && match.packets.front().timestamp <= bg->GetStartTime())
        {
            if (bg->GetPlayers().empty())
                break;

            WorldPacket* myPacket = &match.packets.front().packet;
            Player* replayer = bg->GetPlayers().begin()->second;
            replayer->GetSession()->SendPacket(myPacket);
            match.packets.pop_front();
        }
    }

    /*void OnBattlegroundStart(Battleground* bg) override {
                // Maybe get player ids and classes before (so when a player leaves before OnBattleGroundEnd, we still have the player data)
		// Maybe only record a game if it lasts ~15s seconds?
    }*/

    void OnBattlegroundEnd(Battleground *bg, TeamId winnerTeamId ) override {

        if (!bg->isArena() && !sConfigMgr->GetOption<bool>("ArenaReplay.SaveBattlegrounds", true))
        {
            return;
        }

        if (!bg->isRated() && !sConfigMgr->GetOption<bool>("ArenaReplay.SaveUnratedArenas", true))
        {
            return;
        }

        const bool isReplay = bgReplayIds.find(bg->GetInstanceID()) != bgReplayIds.end();

        // only saves if arena lasted at least X secs (StartDelayTime is included - 60s StartDelayTime + X StartTime)
        uint32 ValidArenaDuration = sConfigMgr->GetOption<uint32>("ArenaReplay.ValidArenaDuration", 75) * IN_MILLISECONDS;
        bool ValidArena = (bg->GetStartTime()) >= ValidArenaDuration || sConfigMgr->GetOption<uint32>("ArenaReplay.ValidArenaDuration", 75) == 0;

        // save replay when a bg ends
        if (!isReplay && ValidArena)
        {
           saveReplay(bg, winnerTeamId);
           return;
        }

        bgReplayIds.erase(bg->GetInstanceID());
    }

    void saveReplay(Battleground* bg, TeamId winnerTeamId)
    {
        //retrieve replay data
        auto it = records.find(bg->GetInstanceID());
        if (it == records.end()) return;
        MatchRecord& match = it->second;

        /** serialize arena replay data **/
        ArenaReplayByteBuffer buffer;
        uint32 headerSize;
        uint32 timestamp;
        for (auto it : match.packets)
        {
            headerSize = it.packet.size(); //header 4Bytes packet size
            timestamp = it.timestamp;

            buffer << headerSize; //4 bytes
            buffer << timestamp; //4 bytes
            buffer << it.packet.GetOpcode(); // 2 bytes
            if (headerSize > 0)
                buffer.append(it.packet.contents(), it.packet.size()); // headerSize bytes
        }

        uint32 teamWinnerRating = 0;
        uint32 teamLoserRating = 0;
        uint32 teamWinnerMMR = 0;
        uint32 teamLoserMMR = 0;
        std::string teamWinnerName;
        std::string teamLoserName;
        std::string winnerPlayerNames;
        std::string loserPlayerNames;
        std::string winnerClassIds;
        std::string loserClassIds;

        for (const auto& playerPair : bg->GetPlayers())
        {
            Player* player = playerPair.second;
            if (!player)
                continue;

            std::string playerClassId = std::to_string(player->getClass());
            std::string playerName = player->GetName();

            TeamId bgTeamId = player->GetBgTeamId();
            ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(bgTeamId));
            uint32 arenaTeamId = bg->GetArenaTeamIdForTeam(bgTeamId);
            TeamId teamId = static_cast<TeamId>(arenaTeamId);
            uint32 teamMMR = bg->GetArenaMatchmakerRating(teamId);

            if (bgTeamId == winnerTeamId)
            {
                if (!winnerClassIds.empty())
                    winnerClassIds += ", ";
                winnerClassIds += playerClassId;

                if (!winnerPlayerNames.empty())
                    winnerPlayerNames += ", ";
                winnerPlayerNames += playerName;

                if (bg->isRated() && team)
                {
                    if (team->GetId() < 0xFFF00000)
                    {
                        teamWinnerName = team->GetName();
                        teamWinnerRating = team->GetRating();
                        teamWinnerMMR = teamMMR;
                    }
                    // 3v3 Solo Queue match (temporary team to merge players in 1 team)
                    else if (team->GetId() >= 0xFFF00000)
                    {
                        teamWinnerName = "3v3 Solo Queue";
                        teamWinnerRating = team->GetRating();
                        teamWinnerMMR = teamMMR;
                    }
                }
                if (bg->isArena() && !bg->isRated())
                {
                    teamWinnerName = "Skirmish Arena";
                }
                else if (!bg->isArena())
                {
                    teamWinnerName = "Battleground";
                }
            }
            else // Loss
            {
                if (!loserClassIds.empty())
                    loserClassIds += ", ";
                loserClassIds += playerClassId;

                if (!loserPlayerNames.empty())
                    loserPlayerNames += ", ";
                loserPlayerNames += playerName;

                if (bg->isRated() && team)
                {
                    if (team->GetId() < 0xFFF00000)
                    {
                        teamLoserName = team->GetName();
                        teamLoserRating = team->GetRating();
                        teamLoserMMR = teamMMR;
                    }
                    // 3v3 Solo Queue match
                    else if (team->GetId() >= 0xFFF00000)
                    {
                        teamLoserName = "3v3 Solo Queue";
                        teamLoserRating = team->GetRating();
                        teamLoserMMR = teamMMR;
                    }
                }
                if (bg->isArena() && !bg->isRated())
                {
                    teamLoserName = "Skirmish Arena";
                }
                else if (!bg->isArena())
                {
                    teamLoserName = "Battleground";
                }
            }

            // Send replay ID to player after a game end
            uint32 replayfightid = 0;
            QueryResult qResult = CharacterDatabase.Query("SELECT MAX(`id`) AS max_id FROM `character_arena_replays`");
            if (qResult)
            {
                do
                {
                    replayfightid = qResult->Fetch()[0].Get<uint32>();
                } while (qResult->NextRow());
            }
            ChatHandler(player->GetSession()).PSendSysMessage("Replay saved. Match ID: {}", replayfightid + 1);
        }

        // // if loser has a negative value. the uint variable could return this (wrong) value
        // if (teamLoserMMR >= 4294967286)
        //     teamLoserMMR=0;

        // if (teamWinnerMMR >= 4294967286)
        //     teamWinnerMMR=0;

        // temporary code until the issue is not properly fixed
        teamLoserMMR=0;
        teamWinnerMMR=0;

        CharacterDatabase.Execute("INSERT INTO `character_arena_replays` "
            //   1             2            3            4          5          6                  7                    8                 9
            "(`arenaTypeId`, `typeId`, `contentSize`, `contents`, `mapId`, `winnerTeamName`, `winnerTeamRating`, `winnerTeamMMR`, `winnerClassIds`, "
            //    10                11                12              13                 14                  15
            "`loserTeamName`, `loserTeamRating`, `loserTeamMMR`, `loserClassIds`, `winnerPlayerNames`, `loserPlayerNames`) "

            "VALUES ({}, {}, {}, \"{}\", {}, '{}', {}, {}, \"{}\", '{}', {}, {}, \"{}\", \"{}\", \"{}\")",
            //       1   2    3     4    5    6    7   8     9      10   11  12    13     14       15

            uint32(match.arenaTypeId), // 1
            uint32(match.typeId),      // 2
            buffer.size(),             // 3
            Acore::Encoding::Base32::Encode(buffer.contentsAsVector()), // 4
            bg->GetMapId(),    // 5
            teamWinnerName,    // 6
            teamWinnerRating,  // 7
            teamWinnerMMR,     // 8
            winnerClassIds,    // 9
            teamLoserName,     // 10
            teamLoserRating,   // 11
            teamLoserMMR,      // 12
            loserClassIds,     // 13
            winnerPlayerNames, // 14
            loserPlayerNames   // 15
        );

        records.erase(it);
    }
};

enum ReplayGossips
{
    REPLAY_LATEST_2V2 = 1,
    REPLAY_LATEST_3V3 = 2,
    REPLAY_LATEST_5V5 = 3,
    REPLAY_LATEST_3V3SOLO = 4,
    REPLAY_LATEST_1V1 = 5,
    REPLAY_MATCH_ID = 6,
    REPLAY_LIST_BY_PLAYERNAME = 7,
    MY_FAVORITE_MATCHES = 8,
    REPLAY_TOP_2V2_ALLTIME = 9,
    REPLAY_TOP_3V3_ALLTIME = 10,
    REPLAY_TOP_5V5_ALLTIME = 11,
    REPLAY_TOP_3V3SOLO_ALLTIME = 12,
    REPLAY_TOP_1V1_ALLTIME = 13,
    REPLAY_MOST_WATCHED_ALLTIME = 14
};

class ReplayGossip : public CreatureScript
{
public:

    ReplayGossip() : CreatureScript("ReplayGossip") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!sConfigMgr->GetOption<bool>("ArenaReplay.Enable", true))
        {
            ChatHandler(player->GetSession()).SendSysMessage("Arena Replay disabled!");
            return true;
        }

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 2v2 games of the last 30 days", GOSSIP_SENDER_MAIN, REPLAY_LATEST_2V2);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 3v3 games of the last 30 days", GOSSIP_SENDER_MAIN, REPLAY_LATEST_3V3);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 5v5 games of the last 30 days", GOSSIP_SENDER_MAIN, REPLAY_LATEST_3V3);

        if (sConfigMgr->GetOption<bool>("ArenaReplay.3v3soloQ.Enable", false)) {
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 3v3 Solo games of the last 30 days", GOSSIP_SENDER_MAIN, REPLAY_LATEST_3V3SOLO);
        }

        if (sConfigMgr->GetOption<bool>("ArenaReplay.1v1.Enable", false)) {
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 1v1 games of the last 30 days", GOSSIP_SENDER_MAIN, REPLAY_LATEST_1V1);
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Replay a Match ID", GOSSIP_SENDER_MAIN, REPLAY_MATCH_ID, "", 0, true);             // maybe add command .replay 'replayID' aswell
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Replay list by player name", GOSSIP_SENDER_MAIN, REPLAY_LIST_BY_PLAYERNAME, "", 0, true); // to do: show a list, showing games with type, teamname and teamrating
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "My favorite matches", GOSSIP_SENDER_MAIN, MY_FAVORITE_MATCHES);                   // To do: somehow show teamName/TeamRating/Classes (it's a different db table)
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 2v2 games of all time", GOSSIP_SENDER_MAIN, REPLAY_TOP_2V2_ALLTIME);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 3v3 games of all time", GOSSIP_SENDER_MAIN, REPLAY_TOP_3V3_ALLTIME);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 5v5 games of all time", GOSSIP_SENDER_MAIN, REPLAY_TOP_5V5_ALLTIME);

        if (sConfigMgr->GetOption<bool>("ArenaReplay.3v3soloQ.Enable", false)) {
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 3v3 Solo games of all time", GOSSIP_SENDER_MAIN, REPLAY_TOP_3V3SOLO_ALLTIME);
        }

        if (sConfigMgr->GetOption<bool>("ArenaReplay.1v1.Enable", false)) {
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay top 1v1 games of all time", GOSSIP_SENDER_MAIN, REPLAY_TOP_1V1_ALLTIME);
        }

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay most watched games of all time", GOSSIP_SENDER_MAIN, REPLAY_MOST_WATCHED_ALLTIME);  // To Do: show arena type + watchedTimes, maybe hide team name
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /* sender */, uint32 action) override
    {
        const uint8 ARENA_TYPE_1v1 = sConfigMgr->GetOption<uint>("ArenaReplay.1v1.ArenaType", 1);
        const uint8 ARENA_TYPE_3V3_SOLO_QUEUE = sConfigMgr->GetOption<uint>("ArenaReplay.3v3soloQ.ArenaType", 4);

        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case REPLAY_LATEST_2V2:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysLast30Days(player, creature, ARENA_TYPE_2v2);
                break;
            case REPLAY_LATEST_3V3:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysLast30Days(player, creature, ARENA_TYPE_3v3);
                break;
            case REPLAY_LATEST_5V5:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysLast30Days(player, creature, ARENA_TYPE_5v5);
                break;
            case REPLAY_LATEST_3V3SOLO:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysLast30Days(player, creature, ARENA_TYPE_3V3_SOLO_QUEUE);
                break;
            case REPLAY_LATEST_1V1:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysLast30Days(player, creature, ARENA_TYPE_1v1);
                break;
            case REPLAY_TOP_2V2_ALLTIME:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysAllTime(player, creature, ARENA_TYPE_2v2);
                break;
            case REPLAY_TOP_3V3_ALLTIME:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysAllTime(player, creature, ARENA_TYPE_3v3);
                break;
            case REPLAY_TOP_5V5_ALLTIME:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysAllTime(player, creature, ARENA_TYPE_5v5);
                break;
            case REPLAY_TOP_3V3SOLO_ALLTIME:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysAllTime(player, creature, ARENA_TYPE_3V3_SOLO_QUEUE);
                break;
            case REPLAY_TOP_1V1_ALLTIME:
                player->PlayerTalkClass->SendCloseGossip();
                ShowReplaysAllTime(player, creature, ARENA_TYPE_1v1);
                break;
            case REPLAY_MOST_WATCHED_ALLTIME:
                player->PlayerTalkClass->SendCloseGossip();
                ShowMostWatchedReplays(player, creature);
                break;
            case MY_FAVORITE_MATCHES:
                player->PlayerTalkClass->SendCloseGossip();
                ShowSavedReplays(player, creature);
                break;
            case GOSSIP_ACTION_INFO_DEF: // "Back"
                OnGossipHello(player, creature);
                break;

            default:
                if (action >= GOSSIP_ACTION_INFO_DEF + 30) // Replay selected arenas (intid >= 30)
                {
                    return replayArenaMatch(player, action - (GOSSIP_ACTION_INFO_DEF + 30));
                }
        }

        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* /* creature */, uint32 /* sender */, uint32 action, const char* code) override
    {
        if (!code)
        {
            CloseGossipMenuFor(player);
            return false;
        }
        // Forbidden: ', %, and , (' causes crash when using 'Replay list by player name')
        std::string inputCode = std::string(code);
        if (inputCode.find('\'') != std::string::npos || inputCode.find('%') != std::string::npos || inputCode.find(',') != std::string::npos)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Invalid input.");
            CloseGossipMenuFor(player);
            return false;
        }
        if (inputCode.length() > 50)
        {
            CloseGossipMenuFor(player);
            return false;
        }
        if (inputCode.empty())
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Invalid input.");
            CloseGossipMenuFor(player);
            return false;
        }

        switch (action)
        {
            case REPLAY_MATCH_ID:
            {
                uint32 replayId;
                try
                {
                    replayId = std::stoi(code);
                }
                catch (...)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Invalid Match ID.");
                    CloseGossipMenuFor(player);
                    return false;
                }
                return replayArenaMatch(player, replayId);
            }
            case REPLAY_LIST_BY_PLAYERNAME:
            {
                QueryResult result = CharacterDatabase.Query("SELECT id FROM character_arena_replays WHERE winnerPlayerNames LIKE '%" + std::string(code) + "%' OR loserPlayerNames LIKE '%" + std::string(code) + "%'");
                if (result)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Replays found for player: {}", std::string(code));
                    do
                    {
                        Field* fields = result->Fetch();
                        uint32 replayId = fields[0].Get<uint32>();
                        ChatHandler(player->GetSession()).PSendSysMessage("Replay ID: {}", replayId);
                        //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Replay " + std::to_string(replayId), GOSSIP_SENDER_MAIN, 30 + replayId);
                    } while (result->NextRow());
                    CloseGossipMenuFor(player);
                    return true;
                }
                else
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("No replays found for player: {}", std::string(code));
                    CloseGossipMenuFor(player);
                    return false;
                }
            }
            case MY_FAVORITE_MATCHES:
            {
                try
                {
                    uint32 NumberTyped = std::stoi(code);
                    FavoriteMatchId(player->GetGUID().GetCounter(), NumberTyped);
                    return true;
                }
                catch (...)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Invalid Match ID.");
                    CloseGossipMenuFor(player);
                    return false;
                }
            }
        }

        return false;
    }

private:

    std::string GetClassIconById(uint8 id)
    {
        std::string sClass = "";
        switch (id)
        {
            case CLASS_WARRIOR:
                sClass = "|TInterface\\icons\\inv_sword_27";
                break;
            case CLASS_PALADIN:
                sClass = "|TInterface\\icons\\inv_hammer_01";
                break;
            case CLASS_HUNTER:
                sClass = "|TInterface\\icons\\inv_weapon_bow_07";
                break;
            case CLASS_ROGUE:
                sClass = "|TInterface\\icons\\inv_throwingknife_04";
                break;
            case CLASS_PRIEST:
                sClass = "|TInterface\\icons\\inv_staff_30";
                break;
            case CLASS_DEATH_KNIGHT:
                sClass = "|TInterface\\icons\\spell_deathknight_classicon";
                break;
            case CLASS_SHAMAN:
                sClass = "TInterface\\icons\\inv_jewelry_talisman_04";
                break;
            case CLASS_MAGE:
                sClass = "|TInterface\\icons\\inv_staff_13";
                break;
            case CLASS_WARLOCK:
                sClass = "|TInterface\\icons\\spell_nature_drowsy";
                break;
            case CLASS_DRUID:
                sClass = "|TInterface\\icons\\inv_misc_monsterclaw_04";
                break;
        }
        return sClass;
    }

    struct ReplayInfo
    {
        uint32 matchId;
        std::string winnerTeamName;
        uint32 winnerTeamRating;
        std::string winnerClasses;
        std::string loserTeamName;
        uint32 loserTeamRating;
        std::string loserClasses;
        //uint32 winnerMMR;
        //uint32 loserMMR;
    };

    void ShowReplaysAllTime(Player* player, Creature* creature, uint8 arenaTypeId)
    {
        auto matchInfos = loadReplaysAllTimeByArenaType(arenaTypeId);

        if (matchInfos.empty())
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "No replays found for this arena type.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "[Replay ID] (Team Rating) 'Team Name'\n----------------------------------------------", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF); // Back to Main Menu
            for (const auto& info : matchInfos)
            {
                std::string classIconsTextTeam1;
                std::string classIconsTextTeam2;
                std::vector<std::string> classIdsTeam1;
                std::stringstream ssWinnerClasses(info.winnerClasses);
                std::string item;
                while (std::getline(ssWinnerClasses, item, ','))
                {
                    classIdsTeam1.push_back(item);
                }
                std::vector<std::string> classIdsTeam2;
                std::stringstream ssLoserClasses(info.loserClasses);
                while (std::getline(ssLoserClasses, item, ','))
                {
                    classIdsTeam2.push_back(item);
                }

                for (const std::string& classId : classIdsTeam1)
                {
                    uint32 id = std::stoi(classId);
                    classIconsTextTeam1 += GetClassIconById(id) + ":14:14:05:00|t|r";
                }
                for (const std::string& classId : classIdsTeam2)
                {
                    uint32 id = std::stoi(classId);
                    classIconsTextTeam2 += GetClassIconById(id) + ":14:14:05:00|t|r";
                }
                if (!classIconsTextTeam1.empty() && classIconsTextTeam1.back() == '\n')
                    classIconsTextTeam1.pop_back();
                if (!classIconsTextTeam2.empty() && classIconsTextTeam2.back() == '\n')
                    classIconsTextTeam2.pop_back();
                std::string coloredWinnerTeamName = "|cff00ff00" + info.winnerTeamName + "|r";
                std::string LoserTeamName = info.loserTeamName;

                std::string gossipText = ("[" + std::to_string(info.matchId) + "] (" +
                    std::to_string(info.winnerTeamRating) + ")" +
                    classIconsTextTeam1 + "" +
                    " '" + coloredWinnerTeamName + "'" +
                    "\n vs   (" + std::to_string(info.loserTeamRating) + ")" +
                    classIconsTextTeam2 + "" +
                    " '" + LoserTeamName + "'");

                const uint32 actionOffset = GOSSIP_ACTION_INFO_DEF + 30;
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, gossipText, GOSSIP_SENDER_MAIN, actionOffset + info.matchId);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }
    std::vector<ReplayInfo> loadReplaysAllTimeByArenaType(uint8 arenaTypeId)
    {
        std::vector<ReplayInfo> records;
        QueryResult result = CharacterDatabase.Query("SELECT id, winnerTeamName, winnerTeamRating, winnerClassIds, loserTeamName, loserTeamRating, loserClassIds FROM character_arena_replays WHERE arenaTypeId = {} ORDER BY winnerTeamRating DESC LIMIT 20", arenaTypeId);

        if (!result)
            return records;

        do
        {
            Field* fields = result->Fetch();
            if (!fields)
                return records;

            ReplayInfo info;
            info.matchId = fields[0].Get<uint32>();
            info.winnerTeamName = fields[1].Get<std::string>();
            info.winnerTeamRating = fields[2].Get<uint32>();
            info.winnerClasses = fields[3].Get<std::string>();
            info.loserTeamName = fields[4].Get<std::string>();
            info.loserTeamRating = fields[5].Get<uint32>();
            info.loserClasses = fields[6].Get<std::string>();

            records.push_back(info);
        } while (result->NextRow());

        return records;
    }

    void ShowReplaysLast30Days(Player* player, Creature* creature, uint8 arenaTypeId)
    {
        auto matchInfos = loadReplaysLast30Days(arenaTypeId);

        if (matchInfos.empty())
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "No replays found for this arena type.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "[Replay ID] (Team Rating) 'Team Name'\n----------------------------------------------", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF); // Back to Main Menu
            for (const auto& info : matchInfos)
            {
                std::string classIconsTextTeam1;
                std::string classIconsTextTeam2;
                std::vector<std::string> classIdsTeam1;
                std::stringstream ssWinnerClasses(info.winnerClasses);
                std::string item;
                while (std::getline(ssWinnerClasses, item, ','))
                {
                    classIdsTeam1.push_back(item);
                }
                std::vector<std::string> classIdsTeam2;
                std::stringstream ssLoserClasses(info.loserClasses);
                while (std::getline(ssLoserClasses, item, ','))
                {
                    classIdsTeam2.push_back(item);
                }

                for (const std::string& classId : classIdsTeam1)
                {
                    uint32 id = std::stoi(classId);
                    classIconsTextTeam1 += GetClassIconById(id) + ":14:14:05:00|t|r";
                }
                for (const std::string& classId : classIdsTeam2)
                {
                    uint32 id = std::stoi(classId);
                    classIconsTextTeam2 += GetClassIconById(id) + ":14:14:05:00|t|r";
                }
                if (!classIconsTextTeam1.empty() && classIconsTextTeam1.back() == '\n')
                    classIconsTextTeam1.pop_back();
                if (!classIconsTextTeam2.empty() && classIconsTextTeam2.back() == '\n')
                    classIconsTextTeam2.pop_back();
                std::string coloredWinnerTeamName = "|cff00ff00" + info.winnerTeamName + "|r";
                std::string LoserTeamName = info.loserTeamName;

                std::string gossipText = ("[" + std::to_string(info.matchId) + "] (" +
                    std::to_string(info.winnerTeamRating) + ")" +
                    classIconsTextTeam1 + "" +
                    " '" + coloredWinnerTeamName + "'" +
                    "\n vs   (" + std::to_string(info.loserTeamRating) + ")" +
                    classIconsTextTeam2 + "" +
                    " '" + LoserTeamName + "'");

                const uint32 actionOffset = GOSSIP_ACTION_INFO_DEF + 30;
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, gossipText, GOSSIP_SENDER_MAIN, actionOffset + info.matchId);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }
    std::vector<ReplayInfo> loadReplaysLast30Days(uint8 arenaTypeId)
    {
        std::vector<ReplayInfo> records;

        std::time_t now = std::time(nullptr);
        std::tm* tmNow = std::localtime(&now);
        tmNow->tm_mday -= 30;
        std::mktime(tmNow);

        std::stringstream ss;
        ss << std::put_time(tmNow, "%Y-%m-%d %H:%M:%S");
        std::string thirtyDaysAgo = ss.str();

		// Only show games that are 30 days old
        QueryResult result = CharacterDatabase.Query(
            "SELECT id, winnerTeamName, winnerTeamRating, winnerClassIds, loserTeamName, loserTeamRating, loserClassIds, timestamp "
            "FROM character_arena_replays "
            "WHERE arenaTypeId = {} AND timestamp >= '{}' "
            "ORDER BY winnerTeamRating DESC LIMIT 20", arenaTypeId, thirtyDaysAgo.c_str());

        if (!result)
            return records;

        do
        {
            Field* fields = result->Fetch();
            if (!fields)
                return records;

            ReplayInfo info;
            info.matchId = fields[0].Get<uint32>();
            info.winnerTeamName = fields[1].Get<std::string>();
            info.winnerTeamRating = fields[2].Get<uint32>();
            info.winnerClasses = fields[3].Get<std::string>();
            info.loserTeamName = fields[4].Get<std::string>();
            info.loserTeamRating = fields[5].Get<uint32>();
            info.loserClasses = fields[6].Get<std::string>();

            records.push_back(info);
        } while (result->NextRow());

        return records;
    }

    void ShowMostWatchedReplays(Player* player, Creature* creature)
    {
        auto matchInfos = loadMostWatchedReplays();

        if (matchInfos.empty())
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "No replays found.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "[Replay ID] (Team Rating) 'Team Name'\n----------------------------------------------", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF); // Back to Main Menu
            for (const auto& info : matchInfos)
            {
                std::string classIconsTextTeam1;
                std::string classIconsTextTeam2;
                std::vector<std::string> classIdsTeam1;
                std::stringstream ssWinnerClasses(info.winnerClasses);
                std::string item;
                while (std::getline(ssWinnerClasses, item, ','))
                {
                    classIdsTeam1.push_back(item);
                }
                std::vector<std::string> classIdsTeam2;
                std::stringstream ssLoserClasses(info.loserClasses);
                while (std::getline(ssLoserClasses, item, ','))
                {
                    classIdsTeam2.push_back(item);
                }

                for (const std::string& classId : classIdsTeam1)
                {
                    uint32 id = std::stoi(classId);
                    classIconsTextTeam1 += GetClassIconById(id) + ":14:14:05:00|t|r";
                }
                for (const std::string& classId : classIdsTeam2)
                {
                    uint32 id = std::stoi(classId);
                    classIconsTextTeam2 += GetClassIconById(id) + ":14:14:05:00|t|r";
                }
                if (!classIconsTextTeam1.empty() && classIconsTextTeam1.back() == '\n')
                    classIconsTextTeam1.pop_back();
                if (!classIconsTextTeam2.empty() && classIconsTextTeam2.back() == '\n')
                    classIconsTextTeam2.pop_back();
                std::string coloredWinnerTeamName = "|cff00ff00" + info.winnerTeamName + "|r";
                std::string LoserTeamName = info.loserTeamName;

                std::string gossipText = "[" + std::to_string(info.matchId) + "] (" +
                    std::to_string(info.winnerTeamRating) + ")" +
                    classIconsTextTeam1 + "" +
                    " '" + coloredWinnerTeamName + "'" +
                    "\n vs   (" + std::to_string(info.loserTeamRating) + ")" +
                    classIconsTextTeam2 + "" +
                    " '" + LoserTeamName + "'";

                const uint32 actionOffset = GOSSIP_ACTION_INFO_DEF + 30;
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, gossipText, GOSSIP_SENDER_MAIN, actionOffset + info.matchId);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }
    std::vector<ReplayInfo> loadMostWatchedReplays()
    {
        std::vector<ReplayInfo> records;
        QueryResult result = CharacterDatabase.Query(
            "SELECT id, winnerTeamName, winnerTeamRating, winnerClassIds, loserTeamName, loserTeamRating, loserClassIds "
            "FROM character_arena_replays "
            "ORDER BY timesWatched DESC, winnerTeamRating DESC "
            "LIMIT 28");

        if (!result)
            return records;

        do
        {
            Field* fields = result->Fetch();
            if (!fields)
                return records;

            ReplayInfo info;
            info.matchId = fields[0].Get<uint32>();
            info.winnerTeamName = fields[1].Get<std::string>();
            info.winnerTeamRating = fields[2].Get<uint32>();
            info.winnerClasses = fields[3].Get<std::string>();
            info.loserTeamName = fields[4].Get<std::string>();
            info.loserTeamRating = fields[5].Get<uint32>();
            info.loserClasses = fields[6].Get<std::string>();

            records.push_back(info);
        } while (result->NextRow());

        return records;
    }

    void ShowSavedReplays(Player* player, Creature* creature, bool firstPage = true)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Favorite a Match ID", GOSSIP_SENDER_MAIN, MY_FAVORITE_MATCHES, "", 0, true);

        std::string sortOrder = (firstPage) ? "ASC" : "DESC";
        QueryResult result = CharacterDatabase.Query("SELECT replay_id FROM character_saved_replays WHERE character_id = " + std::to_string(player->GetGUID().GetCounter()) + " ORDER BY id " + sortOrder + " LIMIT 29");
        if (!result)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "No saved replays found.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        else
        {
            do
            {
                Field* fields = result->Fetch();
                if (!fields)
                    break;

                uint32 matchId = fields[0].Get<uint32>();
                const uint32 actionOffset = GOSSIP_ACTION_INFO_DEF + 30;
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Replay match " + std::to_string(matchId), GOSSIP_SENDER_MAIN, actionOffset + matchId);

            } while (result->NextRow());

            if (firstPage) // to do
            {
                //AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Next Page", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            }
        }
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }

    void FavoriteMatchId(uint64 playerGuid, uint32 code)
    {
        // Need to check if the match exists in character_arena_replays, then insert in character_saved_replays
        QueryResult result = CharacterDatabase.Query("SELECT id FROM character_arena_replays WHERE id = " + std::to_string(code));
        if (result)
        {
            std::string query = "INSERT INTO character_saved_replays (character_id, replay_id) VALUES (" + std::to_string(playerGuid) + ", " + std::to_string(code) + ")";
            CharacterDatabase.Execute(query.c_str());

            if (Player* player = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(playerGuid)))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Replay match ID {} saved.", code);
                CloseGossipMenuFor(player);
            }
        }
        else
        {
            if (Player* player = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(playerGuid)))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Replay match ID {} does not exist.", code);
                CloseGossipMenuFor(player);
            }
        }
    }

    bool replayArenaMatch(Player* player, uint32 replayId)
    {
        auto handler = ChatHandler(player->GetSession());

        if (!loadReplayDataForPlayer(player, replayId))
        {
            CloseGossipMenuFor(player);
            return false;
        }

        MatchRecord record = loadedReplays[player->GetGUID().GetCounter()];

        Battleground* bg = sBattlegroundMgr->CreateNewBattleground(record.typeId, GetBattlegroundBracketByLevel(record.mapId, sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)), record.arenaTypeId, false);
        if (!bg)
        {
            handler.PSendSysMessage("Couldn't create arena map!");
            handler.SetSentErrorMessage(true);
            return false;
        }

        bgReplayIds[bg->GetInstanceID()] = player->GetGUID().GetCounter();
        player->SetPendingSpectatorForBG(bg->GetInstanceID());
        bg->StartBattleground();

        BattlegroundTypeId bgTypeId = bg->GetBgTypeID();

        TeamId teamId = Player::TeamIdForRace(player->getRace());

        uint32 queueSlot = 0;
        WorldPacket data;

        player->SetBattlegroundId(bg->GetInstanceID(), bgTypeId, queueSlot, true, false, TEAM_NEUTRAL);
        player->SetEntryPoint();
        sBattlegroundMgr->SendToBattleground(player, bg->GetInstanceID(), bgTypeId);
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, queueSlot, STATUS_IN_PROGRESS, 0, bg->GetStartTime(), bg->GetArenaType(), teamId);
        player->GetSession()->SendPacket(&data);
        handler.PSendSysMessage("Replay ID {} begins.", replayId);

        return true;
    }

    bool loadReplayDataForPlayer(Player* p, uint32 matchId)
    {
        QueryResult result = CharacterDatabase.Query("SELECT id, arenaTypeId, typeId, contentSize, contents, mapId, timesWatched FROM character_arena_replays WHERE id = {}", matchId);
        if (!result)
        {
            ChatHandler(p->GetSession()).PSendSysMessage("Replay data not found.");
            CloseGossipMenuFor(p);
            return false;
        }

        Field* fields = result->Fetch();
        if (!fields)
        {
            ChatHandler(p->GetSession()).PSendSysMessage("Replay data not found.");
            CloseGossipMenuFor(p);
            return false;
        }

        // Update 'timesWatched' of a Replay +1 everytime someone watches it
        uint32 timesWatched = fields[6].Get<uint32>();
        timesWatched++;
        CharacterDatabase.Execute("UPDATE character_arena_replays SET timesWatched = {} WHERE id = {}", timesWatched, matchId);

        MatchRecord record;
        deserializeMatchData(record, fields);

        loadedReplays[p->GetGUID().GetCounter()] = std::move(record);
        return true;
    }

    void deserializeMatchData(MatchRecord& record, Field* fields)
    {
        record.arenaTypeId = uint8(fields[1].Get<uint32>());
        record.typeId = BattlegroundTypeId(fields[2].Get<uint32>());
        std::vector<uint8> data = *Acore::Encoding::Base32::Decode(fields[4].Get<std::string>());
        record.mapId = uint32(fields[5].Get<uint32>());
        ByteBuffer buffer;
        buffer.append(&data[0], data.size());

        /** deserialize replay binary data **/
        uint32 packetSize;
        uint32 packetTimestamp;
        uint16 opcode;
        while (buffer.rpos() <= buffer.size() - 1)
        {
            buffer >> packetSize;
            buffer >> packetTimestamp;
            buffer >> opcode;

            WorldPacket packet(opcode, packetSize);

            if (packetSize > 0) {
                std::vector<uint8> tmp(packetSize, 0);
                buffer.read(&tmp[0], packetSize);
                packet.append(&tmp[0], packetSize);
            }

            record.packets.push_back({ packetTimestamp, packet });
        }
    }
};

class ConfigLoaderArenaReplay : public WorldScript
{
public:
    ConfigLoaderArenaReplay() : WorldScript("config_loader_arena_replay") {}
    virtual void OnAfterConfigLoad(bool /*Reload*/) override {
        DeleteOldReplays();
    }

private:
    void DeleteOldReplays() {
        // delete all the replays older than X days
        const auto days = sConfigMgr->GetOption<uint>("ArenaReplay.DeleteReplaysAfterDays", 0);
        if (days > 0)
        {
            std::string addition = "";

            const bool deleteReplays = sConfigMgr->GetOption<bool>("ArenaReplay.DeleteSavedReplays", false);

            if (!deleteReplays) {
                addition = "AND `id` NOT IN (SELECT `replay_id` FROM `character_saved_replays`)";
            }

            const auto query = "DELETE FROM `character_arena_replays` WHERE `timestamp` < (NOW() - INTERVAL " + std::to_string(days) + " DAY) " + addition;
            CharacterDatabase.Execute(query);

            if (deleteReplays) {
                CharacterDatabase.Execute("DELETE FROM `character_saved_replays` WHERE `replay_id` NOT IN (SELECT `id` FROM `character_arena_replays`)");
            }
        }
    }
};

void AddArenaReplayScripts()
{
    new ConfigLoaderArenaReplay();
    new ArenaReplayServerScript();
    new ArenaReplayBGScript();
    new ArenaReplayArenaScript();
    new ReplayGossip();
}
