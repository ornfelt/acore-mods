#include "AntiFarming.h"

class antifarming_commandscript : public CommandScript {
public:
    antifarming_commandscript() : CommandScript("antifarming_commandscript") {}

    std::vector<ChatCommand> GetCommands() const override {

        static std::vector<ChatCommand> HelpDeleteSubCommandTable = {
            { "all",		SEC_GAMEMASTER,		false,	&HandleHelpDeleteAllCommand,	"" },
            { "ID",		    SEC_GAMEMASTER,		false,	&HandleHelpDeleteIDCommand,		"" }
        };

        static std::vector<ChatCommand> HelpCommandSubTable = {
            { "log",		SEC_GAMEMASTER,		false,	&HandleHelpLogCommand,		"" },
            { "delete",		SEC_GAMEMASTER,		false,	NULL,		            	"",	HelpDeleteSubCommandTable }
        };

        static std::vector<ChatCommand> DeleteCommandSubTable = {
            { "all",		SEC_GAMEMASTER,		true,	&HandleDeleteAllCommand,	"" },
            { "ID",		    SEC_GAMEMASTER,		true,	&HandleDeleteIDCommand,		"" }
        };

        static std::vector<ChatCommand> AFSSubCommandTable = {
            { "log",		SEC_GAMEMASTER,		true,	&HandleLogCommand,		    "" },
            { "delete",		SEC_ADMINISTRATOR,	true,	NULL,		            	"", DeleteCommandSubTable },
            { "help",		SEC_GAMEMASTER,		false,	NULL,		            	"", HelpCommandSubTable }
        };


        static std::vector<ChatCommand> commandTable =
        {
            { "afs",	    SEC_GAMEMASTER,		true,	NULL,		            	"",	AFSSubCommandTable }
        };
        return commandTable;
    }

    static bool HandleLogCommand(ChatHandler* handler, const char* args) {
        WorldSession *Session = handler->GetSession();
        uint32 RLimit = 10;
        uint32 i = 0;
        if ((char*)args)
            RLimit = atoi((char*)args);

        if (RLimit == 0 || RLimit > 100)
            RLimit = 10;

        if (sAntiFarming->dataMap.empty()) {
            Session->SendNotification("There are no records");
            return false;
        }

        char msg[250];
        std::string charName;
        std::string accName;

        for (AntiFarming::antiFarmingData::iterator itr = sAntiFarming->dataMap.begin(); itr != sAntiFarming->dataMap.end() && i < RLimit; ++itr, i++) {
            sCharacterCache->GetCharacterNameByGuid(ObjectGuid(itr->first), charName);
            AccountMgr::GetName(sCharacterCache->GetCharacterAccountIdByGuid(ObjectGuid(itr->first)), accName);
            snprintf(msg, 250, "ID: |cFFFFFFFF%lu|r | Character: |cFFFFFFFF%s|r | Account: |cFFFFFFFF%s|r | Warning Level: |cFFFF0000%u|r\n", (long)itr->first, charName.c_str(), accName.c_str(), itr->second);
            handler->PSendSysMessage("%s", msg);
            handler->SetSentErrorMessage(true);
        }
        return true;
    }

    static bool HandleDeleteAllCommand(ChatHandler* handler, const char* /*args*/) {
        WorldSession *Session = handler->GetSession();

        if (sAntiFarming->dataMap.empty()) {
            Session->SendNotification("There are no records!");
            return false;
        }
        else {
            sAntiFarming->dataMap.clear();
            Session->SendAreaTriggerMessage("All records were successfully deleted!");
            return true;
        }
        return true;
    }

    static bool HandleDeleteIDCommand(ChatHandler* handler, const char* args) {
        if (!*args)
            return false;
        WorldSession *Session = handler->GetSession();
        uint32 id = atoi((char*)args);
        AntiFarming::antiFarmingData::iterator it = sAntiFarming->dataMap.find(id);
        if (it == sAntiFarming->dataMap.end()) {
            char msg[250];
            snprintf(msg, 250, "Log with ID \"%u\" doesn't exist!", id);
            Session->SendNotification("%s", msg);
            return false;
        }
        sAntiFarming->dataMap.erase(it);
        Session->SendAreaTriggerMessage("Log with ID \"%u\" was successfully deleted!", id);
        return true;
    }

    static bool HandleHelpLogCommand(ChatHandler* handler, const char* /*args*/) {
        handler->PSendSysMessage("This command allows you to check the Database Log for abusers of the Anti-Farm System \n");
        handler->PSendSysMessage("Entering a number after this command will be used to check the maximum lines shown \n");
        handler->PSendSysMessage("Example : \".afs log 15\" will show the first 15 records, even if there are more.");
        handler->SetSentErrorMessage(true);
        return true;
    }

    static bool HandleHelpDeleteAllCommand(ChatHandler* handler, const char* /*args*/) {
        handler->PSendSysMessage("This command will delete all existing records of abusers \n");
        handler->PSendSysMessage("This command does not have any arguments.");
        handler->SetSentErrorMessage(true);
        return true;
    }

    static bool HandleHelpDeleteIDCommand(ChatHandler* handler, const char* /*args*/) {
        handler->PSendSysMessage("This command will let you delete a specific record from the Database \n");
        handler->PSendSysMessage("Entering a number after this command will act like the ID of the record \n");
        handler->PSendSysMessage("Example : \".afs delete ID 50 \" will delete the record with ID 50.");
        handler->SetSentErrorMessage(true);
        return true;
    }
};

void AddSC_antifarming_commandscript() {
    new antifarming_commandscript();
}