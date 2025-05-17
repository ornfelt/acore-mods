#include "AntiFarming.h"

using namespace Acore::ChatCommands;

class antifarming_commandscript : public CommandScript
{
public:
    antifarming_commandscript() : CommandScript("antifarming_commandscript") {}

    ChatCommandTable GetCommands() const override
    {

        static ChatCommandTable HelpDeleteSubCommandTable =
        {
            { "all", HandleHelpDeleteAllCommand, SEC_GAMEMASTER, Console::No },
            { "ID",  HandleHelpDeleteIDCommand,  SEC_GAMEMASTER, Console::No }
        };

        static ChatCommandTable HelpCommandSubTable =
        {
            { "log",    HandleHelpLogCommand, SEC_GAMEMASTER, Console::No },
            { "delete", HelpDeleteSubCommandTable }
        };

        static ChatCommandTable DeleteCommandSubTable =
        {
            { "all", HandleDeleteAllCommand, SEC_ADMINISTRATOR, Console::Yes },
            { "ID",  HandleDeleteIDCommand,  SEC_ADMINISTRATOR, Console::Yes }
        };

        static ChatCommandTable AFSSubCommandTable =
        {
            { "log",    HandleLogCommand, SEC_GAMEMASTER, Console::Yes },
            { "delete", DeleteCommandSubTable },
            { "help",   HelpCommandSubTable }
        };

        static ChatCommandTable commandTable =
        {
            { "afs", AFSSubCommandTable }
        };
        return commandTable;
    }

    static bool HandleLogCommand(ChatHandler* handler, std::optional<uint32> RLimit)
    {
        uint32 i = 0;

        if (!RLimit.has_value() || RLimit == 0 || RLimit > 100)
            RLimit = 10;

        if (sAntiFarming->dataMap.empty())
        {
            handler->SendNotification("There are no records");
            return false;
        }

        char msg[250];
        std::string charName;
        std::string accName;

        for (AntiFarming::antiFarmingData::iterator itr = sAntiFarming->dataMap.begin(); itr != sAntiFarming->dataMap.end() && i < RLimit; ++itr, i++)
        {
            sCharacterCache->GetCharacterNameByGuid(ObjectGuid(itr->first), charName);
            AccountMgr::GetName(sCharacterCache->GetCharacterAccountIdByGuid(ObjectGuid(itr->first)), accName);
            snprintf(msg, 250, "ID: |cFFFFFFFF%lu|r | Character: |cFFFFFFFF%s|r | Account: |cFFFFFFFF%s|r | Warning Level: |cFFFF0000%u|r\n", (long)itr->first, charName.c_str(), accName.c_str(), itr->second);
            handler->PSendSysMessage("{}", msg);
            handler->SetSentErrorMessage(true);
        }
        return true;
    }

    static bool HandleDeleteAllCommand(ChatHandler* handler)
    {
        WorldSession *Session = handler->GetSession();

        if (sAntiFarming->dataMap.empty())
        {
            handler->SendNotification("There are no records!");
            return false;
        }
        else
        {
            sAntiFarming->dataMap.clear();
            Session->SendAreaTriggerMessage("All records were successfully deleted!");
            return true;
        }
        return true;
    }

    static bool HandleDeleteIDCommand(ChatHandler* handler, uint32 id)
    {
        WorldSession *Session = handler->GetSession();
        AntiFarming::antiFarmingData::iterator it = sAntiFarming->dataMap.find(id);
        if (it == sAntiFarming->dataMap.end())
        {
            char msg[250];
            snprintf(msg, 250, "Log with ID \"%u\" doesn't exist!", id);
            handler->SendNotification("{}", msg);
            return false;
        }
        sAntiFarming->dataMap.erase(it);
        Session->SendAreaTriggerMessage("Log with ID \"%u\" was successfully deleted!", id);
        return true;
    }

    static bool HandleHelpLogCommand(ChatHandler* handler)
    {
        handler->PSendSysMessage("This command allows you to check the Database Log for abusers of the Anti-Farm System \n");
        handler->PSendSysMessage("Entering a number after this command will be used to check the maximum lines shown \n");
        handler->PSendSysMessage("Example : \".afs log 15\" will show the first 15 records, even if there are more.");
        handler->SetSentErrorMessage(true);
        return true;
    }

    static bool HandleHelpDeleteAllCommand(ChatHandler* handler)
    {
        handler->PSendSysMessage("This command will delete all existing records of abusers \n");
        handler->PSendSysMessage("This command does not have any arguments.");
        handler->SetSentErrorMessage(true);
        return true;
    }

    static bool HandleHelpDeleteIDCommand(ChatHandler* handler)
    {
        handler->PSendSysMessage("This command will let you delete a specific record from the Database \n");
        handler->PSendSysMessage("Entering a number after this command will act like the ID of the record \n");
        handler->PSendSysMessage("Example : \".afs delete ID 50 \" will delete the record with ID 50.");
        handler->SetSentErrorMessage(true);
        return true;
    }
};

void AddSC_antifarming_commandscript()
{
    new antifarming_commandscript();
}