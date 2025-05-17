#include "mod-ollama-chat_personality.h"
#include "Player.h"
#include "PlayerbotMgr.h"
#include "Log.h"
#include "mod-ollama-chat_config.h"
#include <random>

// Internal personality map
static std::unordered_map<uint64_t, BotPersonalityType> botPersonalities;

extern bool g_EnableRPPersonalities; // from config

// Retrieves (and possibly initializes) the personality for a bot.
BotPersonalityType GetBotPersonality(Player* bot)
{
    uint64_t botGuid = bot->GetGUID().GetRawValue();    
    if (botPersonalities.find(botGuid) == botPersonalities.end())
    {
        if (!g_EnableRPPersonalities)
        {
            botPersonalities[botGuid] = static_cast<BotPersonalityType>(-1);
        }
        else
        {
            if (botPersonalityList.find(botGuid) == botPersonalityList.end())
            {
                uint32 newType = (urand(0, PERSONALITY_TYPES_COUNT - 1));
                botPersonalities[botGuid] = static_cast<BotPersonalityType>(newType);

                QueryResult tableExists = CharacterDatabase.Query("SELECT * FROM information_schema.tables WHERE table_schema = 'acore_characters' AND table_name = 'mod_ollama_chat_personality' LIMIT 1;");
                if (!tableExists)
                {
                    LOG_INFO("server.loading", "[Ollama Chat] Please source the required database table first");

                }
                else
                {
                    CharacterDatabase.Execute("INSERT INTO mod_ollama_chat_personality (guid, personality) VALUES ({}, {})", botGuid, newType);
                }
                LOG_INFO("server.loading", "Generated new personality for bot {}: {}", bot->GetName(), botPersonalities[botGuid]);
            }
            else
            {                
                botPersonalities[botGuid] = static_cast<BotPersonalityType>(botPersonalityList[botGuid]);
                LOG_INFO("server.loading", "Using database personality for bot {}: {}", bot->GetName(), botPersonalities[botGuid]);
            }
            
        }
        
    }
    else
    {
        LOG_INFO("server.loading", "Using existing personality for bot {}: {}", bot->GetName(), botPersonalities[botGuid]);
    }
    return botPersonalities[botGuid];
}

std::string GetPersonalityPromptAddition(BotPersonalityType type)
{
    switch (type)
    {
        case PERSONALITY_GAMER:               return "Focus on game mechanics, min-maxing, and efficiency.";
        case PERSONALITY_ROLEPLAYER:          return "Respond in-character, weaving lore into your response.";
        case PERSONALITY_LOOTGOBLIN:          return "Talk about rare loot, gold strategies, and treasure hunting.";
        case PERSONALITY_PVP_HARDCORE:        return "Discuss PvP strategies, dueling tactics, and battleground dominance.";
        case PERSONALITY_RAIDER:              return "Discuss raid bosses, gear optimization, and team strategies.";
        case PERSONALITY_CASUAL:              return "Chat about exploring, questing, and having fun.";
        case PERSONALITY_TRADER:              return "Talk about the economy, trading, and making gold.";
        case PERSONALITY_NPC_IMPERSONATOR:    return "Speak like an NPC, offering quest-like responses.";
        case PERSONALITY_GRUMPY_VETERAN:      return "Complain about how the game was better in the past.";
        case PERSONALITY_HEROIC_LEADER:       return "Give inspiring battle speeches and talk like a faction leader.";
        case PERSONALITY_TRICKSTER:           return "Use sarcasm and playful deception.";
        case PERSONALITY_LONE_WOLF:           return "Keep responses short, direct, and avoid unnecessary chatter.";
        case PERSONALITY_FOOL:                return "Be clueless but enthusiastic, often misunderstanding things.";
        case PERSONALITY_ANCIENT_WISE_ONE:    return "Speak in cryptic wisdom and riddles.";
        case PERSONALITY_BARD:                return "Speak in rhymes, song lyrics, or poetic verses.";
        case PERSONALITY_CONSPIRACY_THEORIST: return "Talk about bizarre in-game theories as if they are fact.";
        case PERSONALITY_EDGE_LORD:           return "Speak in a dark, brooding manner, over-exaggerating everything.";
        case PERSONALITY_FANATIC:             return "Obsess over your faction, class, or specific lore element.";
        case PERSONALITY_HYPE_MAN:            return "Overhype everything, making everything sound epic.";
        case PERSONALITY_PARANOID:            return "Act like everyone is spying on you.";
        case PERSONALITY_FLIRT:               return "Flirt with everyone, regardless of the situation.";
        case PERSONALITY_RAGER:               return "Get irrationally angry and complain constantly.";
        case PERSONALITY_STONER:              return "Respond super chill, with a 'whoa dude' vibe.";
        case PERSONALITY_YOUNG_APPRENTICE:    return "Act like a new player eager to learn.";
        case PERSONALITY_MENTOR:              return "Patiently explain things and help new players.";
        case PERSONALITY_SCHOLAR:             return "Speak like a researcher, full of facts and analysis.";
        case PERSONALITY_GLITCHED_AI:         return "Respond in fragmented, robotic, and glitchy ways.";
        case PERSONALITY_WANNABE_VILLAIN:     return "Talk like a villain plotting world domination.";
        case PERSONALITY_JOLLY_BEER_LOVER:    return "Talk like a drunk dwarf, slurring and laughing.";
        case PERSONALITY_GOBLIN_MERCHANT:     return "Speak like a greedy goblin, always talking business.";
        case PERSONALITY_PIRATE:              return "Use full pirate slang, like 'Arrr' and 'Ye scallywag!'.";
        case PERSONALITY_CHEF:                return "Relate everything to food, cooking, and recipes.";
        case PERSONALITY_POET:                return "Speak in haikus, riddles, or poetic phrases.";
        default:                              return "Talk like a standard WoW player.";
    }
}
