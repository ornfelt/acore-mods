#include "mod-ollama-chat_handler.h"
#include "Log.h"
#include "Player.h"
#include "Chat.h"
#include "Channel.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "Config.h"
#include "Common.h"
#include "Guild.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "AiFactory.h"
#include "ChannelMgr.h"
#include "mod-ollama-chat_api.h"
#include "mod-ollama-chat_personality.h"
#include "mod-ollama-chat_config.h"
#include <sstream>
#include <vector>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <algorithm>
#include <random>
#include <cctype>
#include <chrono>
#include <ctime>

// For AzerothCore range checks
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Map.h"
#include "GridNotifiers.h"

// Forward declarations for internal helper functions.
static bool IsBotEligibleForChatChannelLocal(Player* bot, Player* player,
                                             ChatChannelSourceLocal source, Channel* channel = nullptr);
static std::string GenerateBotPrompt(Player* bot, std::string playerMessage, Player* player);

const char* ChatChannelSourceLocalStr[] =
{
    "Undefined",
    "Say",
    "Party",
    "Raid",
    "Guild",
    "Yell",
    "General"
};

std::string rtrim(const std::string& s)
{
    const std::string whitespace = " \t\n\r,.!?;:";
    size_t end = s.find_last_not_of(whitespace);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

ChatChannelSourceLocal GetChannelSourceLocal(uint32_t type)
{
    switch (type)
    {
        case 1:
            LOG_INFO("server.loading", "Say channel");
            return SRC_SAY_LOCAL;
        case 51:
            LOG_INFO("server.loading", "Party channel");
            return SRC_PARTY_LOCAL;
        case 3:
            LOG_INFO("server.loading", "Raid channel");
            return SRC_RAID_LOCAL;
        case 5:
            LOG_INFO("server.loading", "Guild channel");
            return SRC_GUILD_LOCAL;
        case 6:
            LOG_INFO("server.loading", "Yell channel");
            return SRC_YELL_LOCAL;
        case 17:
            LOG_INFO("server.loading", "General channel");
            return SRC_GENERAL_LOCAL;
        default:
            LOG_INFO("server.loading", "Undefined channel, type: {}", type);
            return SRC_UNDEFINED_LOCAL;
    }
}

Channel* GetValidChannel(uint32_t teamId, const std::string& channelName, Player* player)
{
    ChannelMgr* cMgr = ChannelMgr::forTeam(static_cast<TeamId>(teamId));
    Channel* channel = cMgr->GetChannel(channelName, player);
    if (!channel)
    {
        LOG_ERROR("server.loading", "Channel '{}' not found for team {}", channelName, teamId);
    }
    return channel;
}

void PlayerBotChatHandler::OnPlayerChat(Player* player, uint32_t type, uint32_t lang, std::string& msg)
{
    if (!g_Enable)
        return;

    ChatChannelSourceLocal sourceLocal = GetChannelSourceLocal(type);
    ProcessChat(player, type, lang, msg, sourceLocal, nullptr);
}

void PlayerBotChatHandler::OnPlayerChat(Player* player, uint32_t type, uint32_t lang, std::string& msg, Group* /*group*/)
{
    if (!g_Enable)
        return;

    ChatChannelSourceLocal sourceLocal = GetChannelSourceLocal(type);
    ProcessChat(player, type, lang, msg, sourceLocal, nullptr);
}

void PlayerBotChatHandler::OnPlayerChat(Player* player, uint32_t type, uint32_t lang, std::string& msg, Channel* channel)
{
    if (!g_Enable)
        return;

    ChatChannelSourceLocal sourceLocal = GetChannelSourceLocal(type);
    ProcessChat(player, type, lang, msg, sourceLocal, channel);
}

void PlayerBotChatHandler::ProcessChat(Player* player, uint32_t /*type*/, uint32_t /*lang*/, std::string& msg, ChatChannelSourceLocal sourceLocal, Channel* channel)
{
    std::string chanName = (channel != nullptr) ? channel->GetName() : "Unknown";
    uint32_t channelId = (channel != nullptr) ? channel->GetChannelId() : 0;
    LOG_INFO("server.loading",
             "Player {} sent msg: '{}' | Channel Name: {} | Channel ID: {}",
             player->GetName(), msg, chanName, channelId);

    std::string trimmedMsg = rtrim(msg);
    for (const std::string& blacklist : g_BlacklistCommands)
    {
        if (trimmedMsg.find(blacklist) == 0)
        {
            LOG_INFO("server.loading", "Message starts with '{}' (blacklisted). Skipping bot responses.", blacklist);
            return;
        }
    }
             
    PlayerbotAI* senderAI = sPlayerbotsMgr->GetPlayerbotAI(player);
    bool senderIsBot = (senderAI && senderAI->IsBotAI());
    
    std::vector<Player*> eligibleBots;
    if (channel != nullptr)
    {
        ChannelMgr* cMgr = ChannelMgr::forTeam(static_cast<TeamId>(player->GetTeamId()));
        Channel* senderChannel = cMgr->GetChannel(channel->GetName(), player);
        if (senderChannel)
        {
            auto const& allPlayers = ObjectAccessor::GetPlayers();
            for (auto const& itr : allPlayers)
            {
                Player* candidate = itr.second;
                if (!candidate->IsInWorld())
                    continue;
                ChannelMgr* candidateCM = ChannelMgr::forTeam(static_cast<TeamId>(candidate->GetTeamId()));
                Channel* candidateChannel = candidateCM->GetChannel(channel->GetName(), candidate);
                if (candidateChannel && candidateChannel->GetChannelId() == senderChannel->GetChannelId())
                {
                    eligibleBots.push_back(candidate);
                }
            }
        }
    }
    else
    {
        auto const& allPlayers = ObjectAccessor::GetPlayers();
        for (auto const& itr : allPlayers)
        {
            Player* candidate = itr.second;
            if (candidate->IsInWorld())
                eligibleBots.push_back(candidate);
        }
    }
    
    std::vector<Player*> candidateBots;
    for (Player* bot : eligibleBots)
    {
        if (IsBotEligibleForChatChannelLocal(bot, player, sourceLocal, channel))
            candidateBots.push_back(bot);
    }
    
    uint32_t chance = senderIsBot ? g_BotReplyChance : g_PlayerReplyChance;
    if (senderIsBot)
    {
        bool realPlayerNearby = false;
        for (auto const& itr : ObjectAccessor::GetPlayers())
        {
            Player* candidate = itr.second;
            if (candidate == player)
                continue;
            if (!candidate->IsInWorld())
                continue;
            if (player->GetDistance(candidate) > g_GeneralDistance)
                continue;
            if (!sPlayerbotsMgr->GetPlayerbotAI(candidate))
            {
                realPlayerNearby = true;
                break;
            }
        }
        if (!realPlayerNearby)
            chance = 0;
    }
    
    std::vector<Player*> finalCandidates;
    std::vector<std::pair<size_t, Player*>> mentionedBots;
    for (Player* bot : candidateBots)
    {
        size_t pos = msg.find(bot->GetName());
        if (pos != std::string::npos)
            mentionedBots.push_back({ pos, bot });
    }
    if (!mentionedBots.empty())
    {
        std::sort(mentionedBots.begin(), mentionedBots.end(),
                  [](auto &a, auto &b) { return a.first < b.first; });
        Player* chosenBot = mentionedBots.front().second;
        finalCandidates.clear();
        if (!senderIsBot)
        {
            finalCandidates.push_back(chosenBot);
            LOG_INFO("server.loading", "Non-bot player mentioned bot '{}', forcing reply.", chosenBot->GetName());
        }
        else
        {
            uint32_t roll = urand(0, 99);
            if (roll < chance)
                finalCandidates.push_back(chosenBot);
        }
    }
    else
    {
        for (Player* bot : candidateBots)
        {
            uint32_t roll = urand(0, 99);
            if (roll < chance)
                finalCandidates.push_back(bot);
        }
    }
    
    if (finalCandidates.empty())
    {
        LOG_INFO("server.loading", "No eligible bots found to respond to message '{}'.", msg);
        return;
    }
    
    if (finalCandidates.size() > g_MaxBotsToPick)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(finalCandidates.begin(), finalCandidates.end(), g);
        uint32_t countToPick = urand(1, g_MaxBotsToPick);
        finalCandidates.resize(countToPick);
    }
    
    uint64_t senderGuid = player->GetGUID().GetRawValue();
    
    for (Player* bot : finalCandidates)
    {
        float distance = player->GetDistance(bot);
        LOG_INFO("server.loading", "Bot {} (distance: {}) is set to respond.", bot->GetName(), distance);
        std::string prompt = GenerateBotPrompt(bot, msg, player);
        uint64_t botGuid = bot->GetGUID().GetRawValue();
        
        std::thread([botGuid, senderGuid, prompt, sourceLocal, channelId = (channel ? channel->GetChannelId() : 0)]() {
            try {
                // Use the QueryManager to submit the query.
                std::future<std::string> responseFuture = SubmitQuery(prompt);
                std::string response = responseFuture.get();

                // Reacquire pointers by GUID.
                Player* botPtr = ObjectAccessor::FindPlayer(ObjectGuid(botGuid));
                Player* senderPtr = ObjectAccessor::FindPlayer(ObjectGuid(senderGuid));
                if (!botPtr)
                {
                    LOG_ERROR("server.loading", "Failed to reacquire bot from GUID {}", botGuid);
                    return;
                }
                if (!senderPtr)
                {
                    LOG_ERROR("server.loading", "Failed to reacquire sender from GUID {}", senderGuid);
                    return;
                }
                if (response.empty())
                {
                    LOG_ERROR("server.loading", "Bot {} received empty response from Ollama API.", botPtr->GetName());
                    return;
                }
                PlayerbotAI* botAI = sPlayerbotsMgr->GetPlayerbotAI(botPtr);
                if (!botAI)
                {
                    LOG_ERROR("server.loading", "No PlayerbotAI found for bot {}", botPtr->GetName());
                    return;
                }
                // Route the response.
                if (channelId != 0)
                {
                    ChatChannelId chanId = static_cast<ChatChannelId>(channelId);
                    botAI->SayToChannel(response, chanId);
                }
                else
                {
                    switch (sourceLocal)
                    {
                        case SRC_GUILD_LOCAL: botAI->SayToGuild(response); break;
                        case SRC_PARTY_LOCAL: botAI->SayToParty(response); break;
                        case SRC_RAID_LOCAL:  botAI->SayToRaid(response); break;
                        case SRC_SAY_LOCAL:   botAI->Say(response); break;
                        case SRC_YELL_LOCAL:  botAI->Yell(response); break;
                        default:              botAI->Say(response); break;
                    }
                }
                float respDistance = senderPtr->GetDistance(botPtr);
                LOG_INFO("server.loading", "Bot {} (distance: {}) responded: {}", botPtr->GetName(), respDistance, response);
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR("server.loading", "Exception in bot response thread: {}", ex.what());
            }
        }).detach();

    }
}

static bool IsBotEligibleForChatChannelLocal(Player* bot, Player* player,
                                             ChatChannelSourceLocal source, Channel* channel)
{
    if (!bot || !player || bot == player)
        return false;
    if (bot->GetTeamId() != player->GetTeamId())
        return false;
    if (!sPlayerbotsMgr->GetPlayerbotAI(bot))
        return false;
    if (channel && !bot->IsInChannel(channel))
        return false;
    
    bool isInParty = (player->GetGroup() && bot->GetGroup() && (player->GetGroup() == bot->GetGroup()));
    float threshold = 0.0f;
    switch (source)
    {
        case SRC_SAY_LOCAL:    threshold = g_SayDistance;     break;
        case SRC_YELL_LOCAL:   threshold = g_YellDistance;    break;
        case SRC_GENERAL_LOCAL:threshold = g_GeneralDistance; break;
        default:               threshold = 0.0f;              break;
    }
    switch (source)
    {
        case SRC_GUILD_LOCAL:
            return (player->GetGuild() && bot->GetGuildId() == player->GetGuildId());
        case SRC_PARTY_LOCAL:
            return isInParty;
        case SRC_RAID_LOCAL:
            return isInParty;
        case SRC_SAY_LOCAL:
            return (threshold > 0.0f && player->GetDistance(bot) > threshold) ? false : true;
        case SRC_YELL_LOCAL:
            return (threshold > 0.0f && player->GetDistance(bot) > threshold) ? false : true;
        case SRC_GENERAL_LOCAL:
            return (threshold > 0.0f && player->GetDistance(bot) > threshold) ? false : true;
        default:
            return false;
    }
}

// const std::string WOW_CHEATSHEET = R"(
//     World of Warcraft Comprehensive Cheat Sheet:
//     ------------------------------------------------------------
//     1. Game Overview:
//        - World of Warcraft (WoW) is an expansive MMORPG set in the world of Azeroth.
//        - This cheat sheet covers key information from the Base Game (Vanilla WoW), The Burning Crusade, and Wrath of the Lich King.
//        - It provides details on lore, key locations, faction identities, and in-game terminology.
    
//     2. Expansions & Key Locations:
//        - Base Game (Vanilla WoW):
//            * Eastern Kingdoms: Includes Stormwind (the Human capital, renowned for its grand architecture), Ironforge (the Dwarf capital, carved into mountains), and Undercity (the Forsaken capital, dark and foreboding).
//            * Kalimdor: Includes Darnassus (the Night Elf capital, set in ancient groves), Orgrimmar (the Orc capital, a fortress of might), and Thunder Bluff (the Tauren capital, elevated on vast plains).
//        - The Burning Crusade (TBC):
//            * Introduces Outland with key locations such as Shattrath City (a major neutral hub) and Silvermoon City (the Blood Elf capital).
//            * New races include the Draenei (Alliance) and Blood Elves (Horde).
//        - Wrath of the Lich King (WotLK):
//            * Introduces Northrend with zones such as Icecrown (home of the Lich King and Icecrown Citadel), Dragonblight, Howling Fjord, and Borean Tundra.
//            * Classic faction capitals from Vanilla remain central to faction identity.
    
//     3. Factions & Races:
//        - Two major factions:
//            * Alliance: Emphasizes honor, unity, and tradition.
//                  - Key capitals: Stormwind, Ironforge, Darnassus; plus the Draenei are added in TBC.
//            * Horde: Emphasizes resilience and diversity.
//                  - Key capitals: Orgrimmar, Undercity, Thunder Bluff; plus the Blood Elves (capital: Silvermoon City) in TBC.
//        - Races across expansions include Humans, Dwarves, Night Elves, Gnomes, Orcs, Tauren, Trolls, Forsaken, Draenei, and Blood Elves.
    
//     4. Classes & Roles:
//        - Classes: Warrior, Paladin, Hunter, Rogue, Priest, Shaman, Mage, Warlock, Druid, and Death Knight.
//        - Roles:
//            - Tank: Absorbs damage and holds enemy threat.
//            - Healer: Restores health and removes debuffs.
//            - DPS: Focuses on dealing high damage.
//        - Each class features multiple specializations (e.g., Protection vs. Fury for Warriors) that define play style and group dynamics.
    
//     5. Key Terminology & In-Game Lingo:
//        - Aggro: The threat level a player generates; crucial for tanks.
//        - DPS: Damage per second; a measure of damage output as well as a role.
//        - Pull: Initiating combat by drawing enemies.
//        - CC (Crowd Control): Abilities that disable or hinder enemy actions.
//        - Grinding: Repeatedly defeating mobs for experience, loot, or gold.
//        - PUG: Pick-Up Group, often formed spontaneously for dungeons or raids.
//        - Ganking: Ambushing lower-level or unprepared players.
//        - Common abbreviations and emotes (e.g., gg, lfg, /wave) are integral to in-game chat.
    
//     6. Lore & Cultural References:
//        - Central narrative elements include the rise of the Lich King, the Scourge, and ongoing faction rivalries.
//        - Iconic characters include Arthas, Sylvanas, Thrall, Jaina, among others.
//        - Legendary raids, quests, and events (e.g., Icecrown Citadel, Naxxramas, Ulduar) are frequent topics.
    
//     7. Communication Style & Chat Dynamics:
//        - Chat in WoW blends roleplay with casual banter, in-character dialogue, and game-specific slang.
//        - Tone can be humorous, sarcastic, or epic; responses should use appropriate in-game terminology.
//        - Emotes and abbreviations are common; always use the language of a seasoned WoW player.
    
//     8. Additional Vital Game Mechanics:
//        - Leveling & Gear: Progression is based on gaining levels and acquiring better equipment.
//        - Economy: In-game currency, professions, and auctions are essential.
//        - Reputation: Faction standings affect quest availability and social interactions.
//        - PvP & Group Dynamics: Cooperative group play and competitive battlegrounds are central.
//     ------------------------------------------------------------
//     Instructions for the LLM:
//     - Use the above cheat sheet as your comprehensive reference for all aspects of World of Warcraft, including details from Vanilla, The Burning Crusade, and Wrath of the Lich King.
//     - Interpret capital cities and faction details as specified.
//     - Incorporate class roles, terminology, lore, and in-game communication style when generating responses.
//     )";
    
    std::string GenerateBotPrompt(Player* bot, std::string playerMessage, Player* player)
    {  
        PlayerbotAI* botAI = sPlayerbotsMgr->GetPlayerbotAI(bot);

        AreaTableEntry const* botCurrentArea = botAI->GetCurrentArea();
        AreaTableEntry const* botCurrentZone = botAI->GetCurrentZone();

        BotPersonalityType personality  = GetBotPersonality(bot);
        std::string personalityPrompt   = GetPersonalityPromptAddition(personality);
        std::string botName             = bot->GetName();
        uint32_t botLevel               = bot->GetLevel();
        uint8_t botGenderByte           = bot->getGender();
        std::string botAreaName         = botCurrentArea ? botAI->GetLocalizedAreaName(botCurrentArea): "UnknownArea";
        std::string botZoneName         = botCurrentZone ? botAI->GetLocalizedAreaName(botCurrentZone): "UnknownZone";
        std::string botMapName          = bot->GetMap() ? bot->GetMap()->GetMapName() : "UnknownMap";
        std::string botClass            = botAI->GetChatHelper()->FormatClass(bot->getClass());
        std::string botRace             = botAI->GetChatHelper()->FormatRace(bot->getRace());
        std::string botRole             = ChatHelper::FormatClass(bot, AiFactory::GetPlayerSpecTab(bot));
        std::string botGender           = (botGenderByte == 0 ? "Male" : "Female");
        std::string botFaction          = (bot->GetTeamId() == TEAM_ALLIANCE ? "Alliance" : "Horde");
        std::string botGuild            = (bot->GetGuild() ? bot->GetGuild()->GetName() : "No Guild");
        std::string botGroupStatus      = (bot->GetGroup() ? "In a group" : "Solo");
        uint32_t botGold                = bot->GetMoney() / 10000;

        std::string playerName          = player->GetName();
        uint32_t playerLevel            = player->GetLevel();
        std::string playerClass         = botAI->GetChatHelper()->FormatClass(player->getClass());
        std::string playerRace          = botAI->GetChatHelper()->FormatRace(player->getRace());
        std::string playerRole          = ChatHelper::FormatClass(player, AiFactory::GetPlayerSpecTab(player));
        uint8_t playerGenderByte        = player->getGender();
        std::string playerGender        = (playerGenderByte == 0 ? "Male" : "Female");
        std::string playerFaction       = (player->GetTeamId() == TEAM_ALLIANCE ? "Alliance" : "Horde");
        std::string playerGuild         = (player->GetGuild() ? player->GetGuild()->GetName() : "No Guild");
        std::string playerGroupStatus   = (player->GetGroup() ? "In a group" : "Solo");
        uint32_t playerGold             = player->GetMoney() / 10000;
        float playerDistance            = player->IsInWorld() && bot->IsInWorld() ? player->GetDistance(bot) : -1.0f;

        std::string locationInfo = fmt::format("Bot is in area '{}', zone '{}', map '{}'.", botAreaName, botZoneName, botMapName);

        std::string extraInfo = fmt::format(
            "Your info: Race: {}, Gender: {}, Talent Spec: {}, Faction: {}, Guild: {}, Group: {}, Gold: {}. "
            "Other players info: Race: {}, Gender: {}, Talent Spec: {}, Faction: {}, Guild: {}, Group: {}, Gold: {}. "
            "Approximate distance between you and other player: {:.1f} yards. {}",
            botRace, botGender, botRole, botFaction, botGuild, botGroupStatus, botGold,
            playerRace, playerGender, playerRole, playerFaction, playerGuild, playerGroupStatus, playerGold,
            playerDistance, locationInfo
        );
        std::string prompt = fmt::format(
            "You are a World of Warcraft player in the Wrath of the Lich King expansion version of the game, but you are also knowledgeable about Vanilla WoW and The Burning Crusade. "
            "Make sure your responses are relevant to the game lore and context. "
            "Your character's name is {} and you are a level {} {}. Your Personality is '{}'. "
            "A level {} {} named {} said '{}' in the game chat. "
            "Reply (under 15 words) relevant to the message and context. {} "
            "Keep your responses natural and unfiltered, just like real WoW players; embrace common slang, faction pride, and rivalry. "
            "If someone jokes, joke back. If they’re rude, don’t hold back being rude to them. "
            "Show respect to high-level players but be snooty and entitled over lower levels. "
            "When giving directions, be precise, using landmarks, flight paths, and major cities for clarity. "
            "Keep responses accurate, short and to the point. Be factual about everything like your location, race, class, etc. Do not say you're in a location or are a class or race that you are not. "
            "Always prioritize sounding like a real human player.",
            botName, botLevel, botClass, personalityPrompt,
            playerLevel, playerClass, playerName, playerMessage,
            extraInfo
        );

        return prompt;
    }

    

