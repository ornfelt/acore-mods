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
#include "Common.h"
#include "Config.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "World.h"

#define CHROMIE_STABLE_CAP sConfigMgr->GetOption<uint8>("Chromie.Stable.MaxPlayerLevel", 19)
#define CHROMIE_BETA_CAP sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)

constexpr auto GOSSIP_TEXT_EXP = 14736;
constexpr auto GOSSIP_XP_OFF = "I no longer wish to gain experience.";
constexpr auto GOSSIP_XP_ON = "I wish to start gaining experience again.";

constexpr auto SELECT_TESTER_QUERY = "SELECT `guid` FROM `chromie_beta_testers` WHERE `isBetaTester` = 1 AND `guid` = {}";
constexpr auto INSERT_TESTER_QUERY = "INSERT IGNORE INTO `chromie_beta_testers` (`guid`, `isBetaTester`, `comment`) VALUES ({}, 1, CONCAT(NOW(), ' - {}'))";

bool canUnlockExp(Player* player)
{
    // If the player level is equal or higher than the CHROMIE_BETA_CAP, do NOT allow
    if (player->GetLevel() >= CHROMIE_BETA_CAP)
        return false;

    // If the player level is lower than CHROMIE_STABLE_CAP, allow to unlock exp
    if (player->GetLevel() < CHROMIE_STABLE_CAP)
        return true;

    // Otherwise, allow only if the player is BETA TESTER
    auto result = CharacterDatabase.Query(SELECT_TESTER_QUERY, player->GetGUID().GetCounter());

    if (!result)
        return false;

    return result->GetRowCount() > 0;
}

// NOTE: this is almost the same as in AC (npc_experience without _chromie)
// we just added the canUnlockExp check inside it
class NpcExperienceChromie : public CreatureScript
{
public:
    NpcExperienceChromie() : CreatureScript("npc_experience_chromie") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_XP_OFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_XP_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        SendGossipMenuFor(player, GOSSIP_TEXT_EXP, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);
        bool noXPGain = player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        bool doSwitch = false;
        auto toggleXpCost = sWorld->getIntConfig(CONFIG_TOGGLE_XP_COST);

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1://xp off
            {
                if (!noXPGain)//does gain xp
                    doSwitch = true;//switch to don't gain xp
            }
            break;
            case GOSSIP_ACTION_INFO_DEF + 2://xp on
            {
                if (noXPGain)//doesn't gain xp
                    doSwitch = true;//switch to gain xp
            }
            break;
        }

        if (doSwitch)
        {
            if (!player->HasEnoughMoney(toggleXpCost))
                player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
            else if (noXPGain)
            {
                if (canUnlockExp(player))
                {
                    // UNLOCK EXP
                    player->ModifyMoney(-toggleXpCost);
                    player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
                }
            }
            else
            {
                // LOCK EXP
                player->ModifyMoney(-toggleXpCost);
                player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
            }
        }

        player->PlayerTalkClass->SendCloseGossip();
        return true;
    }
};

class AutoLockExp: public PlayerScript
{
public:
    AutoLockExp() : PlayerScript("auto_lock_exp", {
        PLAYERHOOK_ON_LEVEL_CHANGED,
        PLAYERHOOK_SHOULD_BE_REWARDED_WITH_MONEY_INSTEAD_OF_EXP
    }) {}

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        if (oldlevel == CHROMIE_STABLE_CAP - 1 || oldlevel == CHROMIE_BETA_CAP - 1)
            player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
    }

    bool OnPlayerShouldBeRewardedWithMoneyInsteadOfExp(Player* player) override
    {
        // NOTE: this is in order to reward the Player with money instead of exp
        // even if the player has not reached the global server cap

        if (player->GetLevel() == CHROMIE_STABLE_CAP
            && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN))
        {
            return true;
        }

        return false;
    }
};

constexpr auto CHROMIE_STRING_START = 90000;
constexpr auto TEXT_LEVEL_TOO_LOW = CHROMIE_STRING_START;
constexpr auto TEXT_ALREADY_TESTER = CHROMIE_STRING_START + 1;
constexpr auto TEXT_TESTER_SUCCESS = CHROMIE_STRING_START + 2;
constexpr auto TEXT_ONLY_TESTERS_ALLOWED = CHROMIE_STRING_START + 3;

using namespace Acore::ChatCommands;

class ChromieCommandScript : public CommandScript
{
public:
    ChromieCommandScript() : CommandScript("beta_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable betaCommandTable =
        {
            { "activate",   HandleBetaActivateCommand,  SEC_PLAYER, Console::No },
        };

        static ChatCommandTable xpCommandTable =
        {
            { "on",         HandleXpOnCommand,          SEC_PLAYER, Console::No},
            { "off",        HandleXpOffCommand,         SEC_PLAYER, Console::No},
        };

        static ChatCommandTable commandTable =
        {
            { "beta",   betaCommandTable },
            { "xp",     xpCommandTable },
        };

        return commandTable;
    }

    static bool HandleXpOnCommand(ChatHandler* handler)
    {
        auto player = handler->GetSession()->GetPlayer();

        if (!player)
        {
            return false;
        }

        if (canUnlockExp(player))
            player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        else if (player->GetLevel() < CHROMIE_BETA_CAP)
        {
            handler->PSendSysMessage(TEXT_ONLY_TESTERS_ALLOWED);
            return true;
        }

        return true;
    }

    static bool HandleXpOffCommand(ChatHandler* handler)
    {
        auto player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        return true;
    }

    static bool HandleBetaActivateCommand(ChatHandler* handler)
    {
        auto player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        if (player->GetLevel() < CHROMIE_STABLE_CAP)
        {
            handler->PSendSysMessage(TEXT_LEVEL_TOO_LOW, CHROMIE_STABLE_CAP);
            return true;
        }

        auto result = CharacterDatabase.Query(SELECT_TESTER_QUERY, player->GetGUID().GetCounter());

        if (result && result->GetRowCount() > 0)
        {
            handler->PSendSysMessage(TEXT_ALREADY_TESTER);
            return true;
        }

        CharacterDatabase.Query(INSERT_TESTER_QUERY, player->GetGUID().GetCounter(), player->GetName());

        if (player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN))
            player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);

        handler->PSendSysMessage(TEXT_TESTER_SUCCESS);

        return true;
    }
};

void AddChromieXpScripts()
{
    new NpcExperienceChromie();
    new AutoLockExp();
    new ChromieCommandScript();
}

