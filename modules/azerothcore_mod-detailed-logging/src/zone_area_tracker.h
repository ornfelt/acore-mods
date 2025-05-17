#include "Configuration/Config.h"
#include "Group.h"
#include "Player.h"
#include "ScriptMgr.h"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

using std::stringstream;
using std::ifstream;
using std::ofstream;

class ZoneAreaTracker : public PlayerScript
{
public:
    stringstream fullStream;
    bool loggingEnabled = sConfigMgr->GetOption<bool>("ZoneAreaDetailedLogging.enabled", true);
    int logDumpSize = sConfigMgr->GetOption<int>("ZoneAreaDetailedLogging.dumpSize", 0);

    ZoneAreaTracker() : PlayerScript("ZoneAreaTracker", {
        PLAYERHOOK_ON_UPDATE_AREA
    })
    {
        // If the file doesn't exist we will create it and add the appropriate headers
        ifstream ifile("zonearea.log");
        if (!ifile)
        {
            fullStream << "timestamp, player, faction, level, maxhealth, currenthealth, newzone, newarea, ingroup, inraid\n";
            StringDump();
        }
    }

    void OnPlayerUpdateArea(Player* player, uint32 oldArea, uint32 newArea) override;

private:
    int insertCount = 0;

    void StringDump()
    {
        insertCount++;

        if (insertCount > logDumpSize)
        {
            // Dump to log
            ofstream logFile;
            logFile.open("zonearea.log", ofstream::app);
            logFile << fullStream.rdbuf();
            logFile.close();

            // Reset the buffer
            fullStream.str("");
            fullStream.clear();

            // Reset line count
            insertCount = 0;
        }
    }
};

void AddZoneAreaTrackerScripts()
{
    new ZoneAreaTracker();
}
