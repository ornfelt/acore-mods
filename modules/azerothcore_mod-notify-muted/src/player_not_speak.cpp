#include "player_not_speak.h"

void PlayerNotSpeak::OnPlayerChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Player* receiver)
{
    if (!sConfigMgr->GetOption<bool>("PlayerNotSpeak.Enable", false))
        return;

    if (receiver->CanSpeak())
        return;

    uint64 muteTime = receiver->GetSession()->m_muteTime;

    if (muteTime == 0)
        return;

    std::string muteTimeStr = secsToTimeString(muteTime - time(NULL), true);
    std::string nameLink = ChatHandler(receiver->GetSession()).playerLink(receiver->GetName());

    ChatHandler(player->GetSession()).PSendSysMessage(RECEIVER_NOT_SPEAK, nameLink, muteTimeStr);
}
