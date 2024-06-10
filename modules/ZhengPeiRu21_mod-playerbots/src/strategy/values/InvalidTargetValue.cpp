/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "InvalidTargetValue.h"
#include "AttackersValue.h"
#include "Playerbots.h"

bool InvalidTargetValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);
    Unit* enemy = AI_VALUE(Unit*, "enemy player target");
    if (target && enemy && target == enemy && target->IsAlive())
        return false;

    if (target && qualifier == "current target")
    {
        return target->GetMapId() != bot->GetMapId() ||
               !target->IsAlive() ||
               target->IsPolymorphed() ||
               target->IsCharmed() ||
               target->isFeared() ||
               target->HasUnitState(UNIT_STATE_ISOLATED) ||
               target->IsFriendlyTo(bot) ||
               !bot->IsWithinDistInMap(target, sPlayerbotAIConfig->sightDistance) ||
               !bot->IsWithinLOSInMap(target);
    }

    return !target;
}
