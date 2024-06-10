#include "Chat.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Configuration/Config.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "Define.h"
#include "GossipDef.h"
#include "DataMap.h"
class QuickTeleport : public CommandScript{
public:

    QuickTeleport() : CommandScript("QuickTeleport") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> TeleportTable =
        {

            { ".telehome", SEC_PLAYER, false, &HandleHomeTeleportCommand, "" },
            { ".telearena", SEC_PLAYER, false, &HandleArenaTeleportCommand, ""}

        };
        return TeleportTable;
    }

    static bool HandleHomeTeleportCommand(ChatHandler* handler, char const* /* args */) // unusued param args
    {
        Player* me = handler->GetSession()->GetPlayer();
        std::string home = sConfigMgr->GetStringDefault("QuickTeleport.homeLocation", "");
        bool enabled = sConfigMgr->GetBoolDefault("QuickTeleport.enabled", true);

        QueryResult result = WorldDatabase.PQuery("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM game_tele WHERE name = '%s'", home.c_str());

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
            uint32 map = fields[0].GetUInt32();
            float position_x = fields[1].GetFloat();
            float position_y = fields[2].GetFloat();
            float position_z = fields[3].GetFloat();
            float orientation = fields[4].GetFloat();

            me->TeleportTo(map, position_x, position_y, position_z, orientation);
        } while (result->NextRow());
        return true;
    }

    static bool HandleArenaTeleportCommand(ChatHandler* handler, char const* /* args */) // unusued parameter args
    {
        bool enabled = sConfigMgr->GetBoolDefault("QuickTeleport.enabled", false);
        std::string arena = sConfigMgr->GetStringDefault("QuickTeleport.arenaLocation", "");

        QueryResult result = WorldDatabase.PQuery("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM game_tele WHERE name = '%s'", arena.c_str());
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
            uint32 map = fields[0].GetUInt32();
            float position_x = fields[1].GetFloat();
            float position_y = fields[2].GetFloat();
            float position_z = fields[3].GetFloat();
            float orientation = fields[4].GetFloat();

            p->TeleportTo(map, position_x, position_y, position_z, orientation);
        } while (result->NextRow());
        return true;
    }

};

void AddQuickTeleportScripts() {
    new QuickTeleport();
}

