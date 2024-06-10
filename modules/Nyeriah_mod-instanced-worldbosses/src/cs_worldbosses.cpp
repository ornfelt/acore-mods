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

#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"

enum Settings
{
    SETTING_BOSS_TIME,
    SETTING_BOSS_STATUS
};

using namespace Acore::ChatCommands;

class worldbosses_commandscript : public CommandScript
{
public:
    worldbosses_commandscript() : CommandScript("worldbosses_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable worldbossTable =
        {
            { "locks", HandleWorldbossShowLocks, SEC_PLAYER, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "worldboss", SEC_GAMEMASTER,     true,  nullptr, "", worldbossTable },
        };

        return commandTable;
    }

    static bool HandleWorldbossShowLocks(ChatHandler* handler)
    {
        Player* player = handler->GetPlayer();

        std::vector<uint32> bossIds = { 6109, 12397, 14887, 14888, 14889, 14890, 17711, 18728 };

        bool foundLocks = false;

        for (auto token : bossIds)
        {
            if (uint32 currentTimer = player->GetPlayerSetting("mod-instanced-worldbosses#" + Acore::ToString(token), SETTING_BOSS_TIME).value)
            {
                foundLocks = true;
                if (time(nullptr) >= (currentTimer + sConfigMgr->GetOption<uint32>("ModInstancedWorldBosses.ResetTimerSecs", 259200))) // 3 days
                {
                    player->UpdatePlayerSetting("mod-instanced-worldbosses#" + Acore::ToString(token), SETTING_BOSS_TIME, 0);
                    player->UpdatePlayerSetting("mod-instanced-worldbosses#" + Acore::ToString(token), SETTING_BOSS_STATUS, 0);
                    if (CreatureTemplate const* creature = sObjectMgr->GetCreatureTemplate(token))
                    {
                        handler->PSendSysMessage("Your lock for %s has reset.", creature->Name);
                    }
                }
                else
                {
                    if (CreatureTemplate const* creature = sObjectMgr->GetCreatureTemplate(token))
                    {
                        tm tmBan = Acore::Time::TimeBreakdown(currentTimer + sConfigMgr->GetOption<uint32>("ModInstancedWorldBosses.ResetTimerSecs", 259200));
                        handler->PSendSysMessage("Your lock for %s expires in |%02d-%02d-%02d %02d:%02d|.", creature->Name, tmBan.tm_year % 100, tmBan.tm_mon + 1, tmBan.tm_mday, tmBan.tm_hour, tmBan.tm_min);
                    }
                }
            }
        }

        if (!foundLocks)
        {
            handler->SendSysMessage("No world boss locks found.");
        }

        return true;
    }
};

void AddSC_worldbosses_commandscript()
{
    new worldbosses_commandscript();
}
