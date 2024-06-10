#include "zone_area_tracker.h"

void ZoneAreaTracker::OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea)
{
    if (loggingEnabled && (oldArea != newArea))
    {
        stringstream areaStream;

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        Group* group = player->GetGroup();
        std::string inGroup = group ? "true" : "false";

        areaStream << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << ", ";
        areaStream << player->GetName() << ", " << player->GetFaction() << ", " << std::to_string(player->GetLevel()) << ", " << player->GetMaxHealth() << ", " << player->GetHealth() << ", ";
        areaStream << oldArea << ", " << newArea << ", " << inGroup << ", ";

        if (group != nullptr)
        {
            areaStream << group->isRaidGroup();
        }
        else
        {
            areaStream << "false";
        }

        areaStream << "\n";

        fullStream << areaStream.str();
        StringDump();
    }
}
