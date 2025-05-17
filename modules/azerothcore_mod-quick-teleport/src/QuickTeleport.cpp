#include "AccountMgr.h"
#include "Chat.h"
#include "Configuration/Config.h"
#include "Creature.h"
#include "DataMap.h"
#include "Define.h"
#include "GossipDef.h"
#include "Player.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

class QuickTeleport : public CommandScript{
public:

    QuickTeleport() : CommandScript("QuickTeleport") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable TeleportTable =
        {
            { "home", HandleHomeTeleportCommand,   SEC_PLAYER,     Console::No },
            { "arena", HandleArenaTeleportCommand,   SEC_PLAYER,  Console::No }
        };

        static ChatCommandTable commandTable =
        {
            { "fast", TeleportTable},
        };

        return commandTable;
    }

    static bool HandleHomeTeleportCommand(ChatHandler* handler, char const* /* args */) // unusued param args
    {
        Player* me = handler->GetSession()->GetPlayer();
        std::string home = sConfigMgr->GetOption<std::string>("QuickTeleport.homeLocation", "");
        bool enabled = sConfigMgr->GetOption<bool>("QuickTeleport.enabled", true);

        QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM game_tele WHERE name = '{}'", home.c_str());

        if (!enabled)
            return false;

        if (!me)
            return false;

        if (me->IsInCombat())
            return false;

        if (!result)
            return false;

        do
        {
            Field* fields = result->Fetch();
            uint32 map = fields[0].Get<uint32>();
            float position_x = fields[1].Get<float>();
            float position_y = fields[2].Get<float>();
            float position_z = fields[3].Get<float>();
            float orientation = fields[4].Get<float>();

            me->TeleportTo(map, position_x, position_y, position_z, orientation);
        } while (result->NextRow());
        return true;
    }

    static bool HandleArenaTeleportCommand(ChatHandler* handler, char const* /* args */) // unusued parameter args
    {
        bool enabled = sConfigMgr->GetOption<bool>("QuickTeleport.enabled", false);
        std::string arena = sConfigMgr->GetOption<std::string>("QuickTeleport.arenaLocation", "");

        QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM game_tele WHERE name = '{}'", arena.c_str());
        Player* p = handler->GetSession()->GetPlayer();

        if (!enabled)
            return false;

        if (!p)
            return false;

        if (p->IsInCombat())
            return false;

        if (!result)
            return false;

        do
        {
            Field* fields = result->Fetch();
            uint32 map = fields[0].Get<uint32>();
            float position_x = fields[1].Get<float>();
            float position_y = fields[2].Get<float>();
            float position_z = fields[3].Get<float>();
            float orientation = fields[4].Get<float>();

            p->TeleportTo(map, position_x, position_y, position_z, orientation);
        } while (result->NextRow());
        return true;
    }

};

void AddQuickTeleportScripts() {
    new QuickTeleport();
}

