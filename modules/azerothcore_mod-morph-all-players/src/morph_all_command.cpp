#include "Chat.h"
#include "Config.h"
#include "Language.h"
#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSessionMgr.h"

using namespace Acore::ChatCommands;

class MorphAllCommandScript : public CommandScript
{
public:
    MorphAllCommandScript() : CommandScript("MorphAllCommandScript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable CustomCommandTable =
        {
            { "morphall",  HandleMorphAllCommand, SEC_ADMINISTRATOR, Console::No },
            { "demorphall",  HandleDeMorphAllCommand, SEC_ADMINISTRATOR, Console::No },
        };
        return CustomCommandTable;
    }

    static bool HandleMorphAllCommand(ChatHandler* /*handler*/, uint32 displayId)
    {

        uint8 configSkipSpecificGmLevel = sConfigMgr->GetOption<uint8>("MorphAll.SkipSpecificGmLevel", 3);

        if (!displayId)
            return false;

        WorldSessionMgr::SessionMap const& m_sessions = sWorldSessionMgr->GetAllSessions();

        for (WorldSessionMgr::SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
                continue;

            // skip depending on player level
            if (itr->second->GetPlayer()->GetSession()->GetSecurity() >= configSkipSpecificGmLevel)
                continue;

            itr->second->GetPlayer()->SetDisplayId(displayId);
        }

        return true;
    }

    static bool HandleDeMorphAllCommand(ChatHandler* /*handler*/)
    {
        WorldSessionMgr::SessionMap const& m_sessions = sWorldSessionMgr->GetAllSessions();

        for (WorldSessionMgr::SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
                continue;

            itr->second->GetPlayer()->DeMorph();
        }

        return true;
    }
};

class MorphAllCommandWorldScript : public WorldScript
{
public:
    MorphAllCommandWorldScript() : WorldScript("MorphAllCommandWorldScript", {
        WORLDHOOK_ON_STARTUP,
        WORLDHOOK_ON_AFTER_CONFIG_LOAD
    }) { }

    void OnStartup() override
    {
        // Show this On worldserver startup
        sLog->outMessage("server", LOG_LEVEL_INFO, "== Loaded morph all command ==");
    }

    void OnAfterConfigLoad(bool reload) override
    {
        if (reload)
        {
            sLog->outMessage("server", LOG_LEVEL_INFO, "== Re-Loaded morph_all_command ==");
            ChatHandler(nullptr).SendGMText(40000);
        }
    }
};

void AddSC_morph_all_command()
{
    new MorphAllCommandScript();
    new MorphAllCommandWorldScript();
}
