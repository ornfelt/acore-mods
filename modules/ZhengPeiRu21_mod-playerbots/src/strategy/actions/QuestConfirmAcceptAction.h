/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_QUESTCONFIRMACCEPTACTION_H
#define _PLAYERBOT_QUESTCONFIRMACCEPTACTION_H

#include "AiObjectContext.h"
#include "PlayerbotAI.h"
#include "QuestAction.h"
#include "Player.h"

class ObjectGuid;
class Quest;
class Player;
class PlayerbotAI;
class WorldObject;

class QuestConfirmAcceptAction : public QuestAction
{
    public:
        QuestConfirmAcceptAction(PlayerbotAI* botAI) : QuestAction(botAI, "quest confirm accept") {}
        bool Execute(Event event) override;
};

#endif