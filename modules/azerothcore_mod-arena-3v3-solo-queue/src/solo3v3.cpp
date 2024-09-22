/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "solo3v3.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "Config.h"
#include "Log.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "DisableMgr.h"

Solo3v3* Solo3v3::instance()
{
    static Solo3v3 instance;
    return &instance;
}

uint32 Solo3v3::GetAverageMMR(ArenaTeam* team)
{
    if (!team)
        return 0;

    // this could be improved with a better balanced calculation
    uint32 matchMakerRating = team->GetStats().Rating;

    return matchMakerRating;
}

void Solo3v3::CleanUp3v3SoloQ(Battleground* bg)
{
    // Cleanup temp arena teams for solo 3v3
    if (bg->isArena() && bg->isRated() && bg->GetArenaType() == ARENA_TYPE_3v3_SOLO)
    {
        ArenaTeam* tempAlliArenaTeam = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_ALLIANCE));
        ArenaTeam* tempHordeArenaTeam = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_HORDE));

        if (tempAlliArenaTeam && tempAlliArenaTeam->GetId() >= MAX_ARENA_TEAM_ID)
        {
            sArenaTeamMgr->RemoveArenaTeam(tempAlliArenaTeam->GetId());
            delete tempAlliArenaTeam;
        }

        if (tempHordeArenaTeam && tempHordeArenaTeam->GetId() >= MAX_ARENA_TEAM_ID)
        {
            sArenaTeamMgr->RemoveArenaTeam(tempHordeArenaTeam->GetId());
            delete tempHordeArenaTeam;
        }
    }
}

void Solo3v3::CheckStartSolo3v3Arena(Battleground* bg)
{
    // Fix crash with Arena Replay module
    for (const auto& playerPair : bg->GetPlayers())
    {
        Player* player = playerPair.second;
        if (player->IsSpectator())
            return;
    }

    if (bg->GetArenaType() != ARENA_TYPE_3v3_SOLO)
        return;

    if (bg->GetStatus() != STATUS_IN_PROGRESS)
        return; // if CheckArenaWinConditions ends the game

    bool someoneNotInArena = false;

    ArenaTeam* team[2];
    team[0] = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_ALLIANCE));
    team[1] = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdForTeam(TEAM_HORDE));

    ASSERT(team[0] && team[1]);

    for (int i = 0; i < 2; i++)
    {
        for (auto const& itr : team[i]->GetMembers())
        {
            Player* plr = ObjectAccessor::FindPlayer(itr.Guid);
            if (!plr)
            {
                someoneNotInArena = true;
                continue;
            }

            if (plr->GetInstanceId() != bg->GetInstanceID())
            {
                if (sConfigMgr->GetOption<bool>("Solo.3v3.CastDeserterOnAfk", true))
                    plr->CastSpell(plr, 26013, true); // Deserter

                someoneNotInArena = true;
            }
        }
    }

    if (someoneNotInArena && sConfigMgr->GetOption<bool>("Solo.3v3.StopGameIncomplete", false))
    {
        bg->SetRated(false);
        bg->EndBattleground(TEAM_NEUTRAL);
    }
}

bool Solo3v3::CheckSolo3v3Arena(BattlegroundQueue* queue, BattlegroundBracketId bracket_id, bool isRated)
{
    bool soloTeam[BG_TEAMS_COUNT][MAX_TALENT_CAT]; // 2 teams and each team 3 players - set to true when slot is taken

    for (int i = 0; i < BG_TEAMS_COUNT; i++)
        for (int j = 0; j < MAX_TALENT_CAT; j++)
            soloTeam[i][j] = false; // default false = slot not taken

    queue->m_SelectionPools[TEAM_ALLIANCE].Init();
    queue->m_SelectionPools[TEAM_HORDE].Init();

    uint32 MinPlayersPerTeam = 3;

    bool filterTalents = sConfigMgr->GetOption<bool>("Solo.3v3.FilterTalents", false);

    for (int teamId = 0; teamId < 2; teamId++) // BG_QUEUE_PREMADE_ALLIANCE and BG_QUEUE_PREMADE_HORDE
    {
        int index = teamId;

        if (!isRated)
            index += PVP_TEAMS_COUNT;

        for (BattlegroundQueue::GroupsQueueType::iterator itr = queue->m_QueuedGroups[bracket_id][index].begin(); itr != queue->m_QueuedGroups[bracket_id][index].end(); ++itr)
        {
            if ((*itr)->IsInvitedToBGInstanceGUID) // Skip when invited
                continue;

            for (auto const& playerGuid : (*itr)->Players)
            {
                Player* plr = ObjectAccessor::FindPlayer(playerGuid);
                if (!plr)
                    continue;

                if (!filterTalents && queue->m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() + queue->m_SelectionPools[TEAM_HORDE].GetPlayerCount() == MinPlayersPerTeam * 2)
                    return true;

                if (!plr)
                    return false;

                Solo3v3TalentCat playerSlotIndex;
                if (sConfigMgr->GetOption<bool>("Solo.3v3.FilterTalents", false))
                    playerSlotIndex = GetTalentCatForSolo3v3(plr);
                else
                    playerSlotIndex = GetFirstAvailableSlot(soloTeam);

                // is slot free in alliance team?
                if ((filterTalents && soloTeam[TEAM_ALLIANCE][playerSlotIndex] == false) || (!filterTalents && queue->m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() != MinPlayersPerTeam))
                {
                    if (queue->m_SelectionPools[TEAM_ALLIANCE].AddGroup((*itr), MinPlayersPerTeam)) // added successfully?
                    {
                        soloTeam[TEAM_ALLIANCE][playerSlotIndex] = true; // okay take this slot

                        if ((*itr)->teamId != TEAM_ALLIANCE) // move to other team
                        {
                            (*itr)->teamId = TEAM_ALLIANCE;
                            (*itr)->GroupType = BG_QUEUE_PREMADE_ALLIANCE;
                            queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].push_front((*itr));
                            itr = queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].erase(itr);
                            return CheckSolo3v3Arena(queue, bracket_id, isRated);
                        }
                    }
                }
                else if ((filterTalents && soloTeam[TEAM_HORDE][playerSlotIndex] == false) || !filterTalents) // nope? and in horde team?
                {
                    if (queue->m_SelectionPools[TEAM_HORDE].AddGroup((*itr), MinPlayersPerTeam))
                    {
                        soloTeam[TEAM_HORDE][playerSlotIndex] = true;

                        if ((*itr)->teamId != TEAM_HORDE) // move to other team
                        {
                            (*itr)->teamId = TEAM_HORDE;
                            (*itr)->GroupType = BG_QUEUE_PREMADE_HORDE;
                            queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].push_front((*itr));
                            itr = queue->m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].erase(itr);
                            return CheckSolo3v3Arena(queue, bracket_id, isRated);
                        }
                    }
                }
            }
        }
    }

    uint32 countAll = 0;
    for (int i = 0; i < BG_TEAMS_COUNT; i++)
        for (int j = 0; j < MAX_TALENT_CAT; j++)
            if (soloTeam[i][j])
                countAll++;

    return countAll == MinPlayersPerTeam * 2 || (!filterTalents && queue->m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() + queue->m_SelectionPools[TEAM_HORDE].GetPlayerCount() == MinPlayersPerTeam * 2);
}

void Solo3v3::CreateTempArenaTeamForQueue(BattlegroundQueue* queue, ArenaTeam* arenaTeams[])
{
    // Create temp arena team
    for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
    {
        ArenaTeam* tempArenaTeam = new ArenaTeam();  // delete it when all players have left the arena match. Stored in sArenaTeamMgr
        std::vector<Player*> playersList;
        uint32 atPlrItr = 0;

        for (auto const& itr : queue->m_SelectionPools[TEAM_ALLIANCE + i].SelectedGroups)
        {
            if (atPlrItr >= 3)
                break; // Should never happen

            for (auto const& itr2 : itr->Players)
            {
                auto _PlayerGuid = itr2;
                if (Player * _player = ObjectAccessor::FindPlayer(_PlayerGuid))
                {
                    playersList.push_back(_player);
                    atPlrItr++;
                }

                break;
            }
        }

        std::stringstream ssTeamName;
        ssTeamName << "Solo Team - " << (i + 1);

        tempArenaTeam->CreateTempArenaTeam(playersList, ARENA_TYPE_3v3_SOLO, ssTeamName.str());
        sArenaTeamMgr->AddArenaTeam(tempArenaTeam);
        arenaTeams[i] = tempArenaTeam;
    }
}

bool Solo3v3::Arena3v3CheckTalents(Player* player)
{
    if (!player)
        return false;

    if (!sConfigMgr->GetOption<bool>("Arena.3v3.BlockForbiddenTalents", false))
        return true;

    uint32 count = 0;
    for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);

        if (!talentInfo)
            continue;

        for (int8 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            if (talentInfo->RankID[rank] == 0)
                continue;

            if (player->HasTalent(talentInfo->RankID[rank], player->GetActiveSpec()))
            {
                for (int8 i = 0; FORBIDDEN_TALENTS_IN_1V1_ARENA[i] != 0; i++)
                    if (FORBIDDEN_TALENTS_IN_1V1_ARENA[i] == talentInfo->TalentTab)
                        count += rank + 1;
            }
        }
    }

    if (count >= 36)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You can't join, because you have invested to much points in a forbidden talent. Please edit your talents.");
        return false;
    }

    return true;
}

Solo3v3TalentCat Solo3v3::GetTalentCatForSolo3v3(Player* player)
{
    uint32 count[MAX_TALENT_CAT];

    for (int i = 0; i < MAX_TALENT_CAT; i++)
        count[i] = 0;

    for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);

        if (!talentInfo)
            continue;

        for (int8 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            if (talentInfo->RankID[rank] == 0)
                continue;

            if (player->HasTalent(talentInfo->RankID[rank], player->GetActiveSpec()))
            {
                for (int8 i = 0; SOLO_3V3_TALENTS_MELEE[i] != 0; i++)
                    if (SOLO_3V3_TALENTS_MELEE[i] == talentInfo->TalentTab)
                        count[MELEE] += rank + 1;

                for (int8 i = 0; SOLO_3V3_TALENTS_RANGE[i] != 0; i++)
                    if (SOLO_3V3_TALENTS_RANGE[i] == talentInfo->TalentTab)
                        count[RANGE] += rank + 1;

                for (int8 i = 0; SOLO_3V3_TALENTS_HEAL[i] != 0; i++)
                    if (SOLO_3V3_TALENTS_HEAL[i] == talentInfo->TalentTab)
                        count[HEALER] += rank + 1;
            }
        }
    }

    uint32 prevCount = 0;

    Solo3v3TalentCat talCat = MELEE; // Default MELEE (if no talent points set)

    for (int i = 0; i < MAX_TALENT_CAT; i++)
    {
        if (count[i] > prevCount)
        {
            talCat = (Solo3v3TalentCat)i;
            prevCount = count[i];
        }
    }

    return talCat;
}

Solo3v3TalentCat Solo3v3::GetFirstAvailableSlot(bool soloTeam[][MAX_TALENT_CAT]) {
    if (!soloTeam[0][MELEE] || !soloTeam[1][MELEE])
        return MELEE;

    if (!soloTeam[0][RANGE] || !soloTeam[1][RANGE])
        return RANGE;

    if (!soloTeam[0][HEALER] || !soloTeam[1][HEALER])
        return HEALER;

    return MELEE;
}
