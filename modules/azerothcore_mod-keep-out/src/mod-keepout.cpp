#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"
#include "Chat.h"

struct MKO
{
    uint32 maxWarnings;
    bool keepOutEnabled;
    bool teleportEnabled;
    bool kickEnabled;
};

MKO mko;

void teleportPlayer(Player* player)
{
    if (player->GetTeamId() == TEAM_HORDE)
    {
        /* Orgrimmar */
        player->TeleportTo(1, 1629.85f, -4373.64f, 31.5573f, 3.69762f);
    }
    else
    {
        /* Stormwind */
        player->TeleportTo(0, -8833.38f, 628.628f, 94.0066f, 1.06535f);
    }
    ChatHandler(player->GetSession()).PSendSysMessage("You have gone to a forbidden place your actions have been logged.");
}

void checkZoneKeepOut(Player* player)
{
    if (player->GetSession()->GetSecurity() >= SEC_MODERATOR)
        return;

    uint32 mapId = player->GetMapId();
    uint32 zoneId = player->GetZoneId();

    QueryResult result = WorldDatabase.Query("SELECT * FROM `mod_mko_map_lock` WHERE `mapId`={} AND `zoneID`={}", mapId, zoneId);

    if (!result)
        return;

    uint32 accountId = player->GetSession()->GetAccountId();
    uint8 countWarnings = 1;

    QueryResult playerWarning = CharacterDatabase.Query("SELECT * FROM `mod_mko_map_exploit` WHERE `accountId`={}", accountId);

    if (!playerWarning)
    {
        CharacterDatabase.Execute("INSERT INTO `mod_mko_map_exploit` (`accountId`, `count`) VALUES ({}, {})", accountId, countWarnings);

        if (mko.teleportEnabled)
        {
            teleportPlayer(player);
        }
    }
    else
    {
        countWarnings = (*playerWarning)[1].Get<uint8>() + 1;

        if (countWarnings <= mko.maxWarnings)
        {
            CharacterDatabase.Execute("UPDATE `mod_mko_map_exploit` SET `count`={} WHERE `accountId`={}", countWarnings, accountId);
            teleportPlayer(player);
        }
        else
        {
            if (mko.teleportEnabled && !mko.kickEnabled)
            {
                teleportPlayer(player);
            }
            else if (mko.kickEnabled)
            {
                player->GetSession()->KickPlayer("MKO: Entering a place not allowed.", true);
            }
            else
            {
                ChatHandler(player->GetSession()).PSendSysMessage("You have gone to a forbidden place your actions have been logged.");
            }
        }
    }
}

class KeepOutPlayerScript : public PlayerScript
{
public:
    KeepOutPlayerScript() : PlayerScript("KeepOutPlayerScript") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("Announcer.Enable", true))
        {
            ChatHandler(player->GetSession()).PSendSysMessage("This server is running the |cff4CFF00Keep Out |rmodule.");
        }
    }

    void OnUpdateZone(Player* player, uint32 /*newZone*/,  uint32 /*newArea*/) override
    {
        if (mko.keepOutEnabled)
        {
            checkZoneKeepOut(player);
        }
    }
};

class KeepOutWorldScript : public WorldScript
{
public:
    KeepOutWorldScript() : WorldScript("KeepOutWorldScript") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload)
        {
            mko.maxWarnings = sConfigMgr->GetOption<int>("MaxWarnings", 3);
            mko.keepOutEnabled = sConfigMgr->GetOption<bool>("KeepOutEnabled", true);
            mko.teleportEnabled = sConfigMgr->GetOption<bool>("KeepOutTeleportEnabled", true);
            mko.kickEnabled = sConfigMgr->GetOption<bool>("KeepOutKickPlayerEnabled", true);
        }
    }
};

void AddKeepOutScripts()
{
    new KeepOutWorldScript();
    new KeepOutPlayerScript();
}
