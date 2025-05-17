#ifndef MOD_OLLAMA_CHAT_CONFIG_H
#define MOD_OLLAMA_CHAT_CONFIG_H

#include <string>
#include <cstdint>
#include <vector>
#include "ScriptMgr.h"  // Ensure WorldScript is defined

extern float      g_SayDistance;
extern float      g_YellDistance;
extern float      g_GeneralDistance;
extern uint32_t   g_PlayerReplyChance;
extern uint32_t   g_BotReplyChance;
extern uint32_t   g_MaxBotsToPick;
extern std::string g_OllamaUrl;
extern std::string g_OllamaModel;
extern std::unordered_map<uint64_t, uint32> botPersonalityList; // New for database personalities

// New configuration option for API concurrent query limit.
extern uint32_t   g_MaxConcurrentQueries;

extern bool       g_Enable;
extern bool       g_EnableRandomChatter;
extern uint32_t   g_MinRandomInterval;
extern uint32_t   g_MaxRandomInterval;
extern float      g_RandomChatterRealPlayerDistance;
extern uint32_t   g_RandomChatterBotCommentChance;

extern bool       g_EnableRPPersonalities;

// Default blacklist for playerbot commands (e.g. if a message starts with any of these, ignore it)
extern std::vector<std::string> g_BlacklistCommands;

// Loads configuration
void LoadOllamaChatConfig();

// Declaration of the configuration WorldScript.
class OllamaChatConfigWorldScript : public WorldScript
{
public:
    OllamaChatConfigWorldScript();
    void OnStartup() override;
};

#endif // MOD_OLLAMA_CHAT_CONFIG_H
