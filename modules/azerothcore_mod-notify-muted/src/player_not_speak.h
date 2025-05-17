#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

enum PlayerNotSpeakLocale
{
    RECEIVER_NOT_SPEAK = 13000
};

class PlayerNotSpeak : public PlayerScript
{
public:
    PlayerNotSpeak() : PlayerScript("PlayerNotSpeak", {
        PLAYERHOOK_ON_CHAT_WITH_RECEIVER
    }) {}
    void OnPlayerChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Player* receiver) override;
};

void AddPlayerNotSpeakScripts()
{
    new PlayerNotSpeak();
}
