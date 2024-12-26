#include "Config.h"
#include "Chat.h"
#include "ScriptMgr.h"
#include "Player.h"

enum PlayerNotSpeakLocale
{
    RECEIVER_NOT_SPEAK = 13000
};

class PlayerNotSpeak : public PlayerScript
{
public:
    PlayerNotSpeak() : PlayerScript("PlayerNotSpeak") {}
    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Player* receiver) override;
};

void AddPlayerNotSpeakScripts()
{
    new PlayerNotSpeak();
}
