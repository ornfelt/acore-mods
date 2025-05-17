#ifndef MOD_OLLAMA_CHAT_HANDLER_H
#define MOD_OLLAMA_CHAT_HANDLER_H

#include "ScriptMgr.h"
#include <string>

enum ChatChannelSourceLocal
{
    SRC_UNDEFINED_LOCAL  = 0,
    SRC_SAY_LOCAL        = 1,
    SRC_PARTY_LOCAL      = 51,
    SRC_RAID_LOCAL       = 3,
    SRC_GUILD_LOCAL      = 4,
    SRC_YELL_LOCAL       = 6,
    SRC_GENERAL_LOCAL    = 17
};

extern const char* ChatChannelSourceLocalStr[];

std::string rtrim(const std::string& s);
ChatChannelSourceLocal GetChannelSourceLocal(uint32_t type);

class PlayerBotChatHandler : public PlayerScript
{
public:
    PlayerBotChatHandler() : PlayerScript("PlayerBotChatHandler") {}
    void OnPlayerChat(Player* player, uint32_t type, uint32_t lang, std::string& msg) override;
    void OnPlayerChat(Player* player, uint32_t type, uint32_t lang, std::string& msg, Group* group) override;
    void OnPlayerChat(Player* player, uint32_t type, uint32_t lang, std::string& msg, Channel* channel) override;

private:
    void ProcessChat(Player* player, uint32_t type, uint32_t lang, std::string& msg, ChatChannelSourceLocal sourceLocal, Channel* channel = nullptr);
};

#endif // MOD_OLLAMA_CHAT_HANDLER_H
