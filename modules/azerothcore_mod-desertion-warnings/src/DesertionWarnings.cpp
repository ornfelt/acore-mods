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

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

class DesertionWarnings : public PlayerScript
{
public:
    DesertionWarnings() : PlayerScript("DesertionWarnings") { }

    void OnBattlegroundDesertion(Player* player, const BattlegroundDesertionType type) override
    {
        if (sConfigMgr->GetOption<bool>("DesertionWarnings.Enabled", false))
        {
            switch (type)
            {
                case BG_DESERTION_TYPE_LEAVE_QUEUE:
                case BG_DESERTION_TYPE_NO_ENTER_BUTTON:
                    ChatHandler(player->GetSession()).PSendSysMessage("{}", sConfigMgr->GetOption<std::string>("DesertionWarnings.WarningText", "").c_str());
                    break;
                default:
                    break;
            }
        }
    }
};

void AddDesertionWarningsScripts()
{
    new DesertionWarnings();
}

