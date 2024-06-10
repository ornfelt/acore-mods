#include "kill_stat_tracker.h"

void KillStatTracker::OnCreatureKill(Player* player, Creature* creature)
{
    if (loggingEnabled)
    {
        stringstream killStream;

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        killStream << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << ", ";
        killStream << player->GetName() << ", " << player->GetFaction() << ", " << std::to_string(player->GetLevel()) << ", " << player->GetMaxHealth() << ", " << player->GetHealth() << ", ";
        killStream << creature->GetName() << ", " << creature->GetFaction() << ", " << creature->GetMaxHealth() << ", ";
        killStream << player->GetZoneId() << ", " << player->GetAreaId() << ", " << player->IsGameMaster();
        killStream << "\n";

        fullStream << killStream.str();
        StringDump();
    }
}

void KillStatTracker::OnCreatureKilledByPet(Player* player, Creature* creature)
{
    if (loggingEnabled)
    {
        stringstream killStream;

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        killStream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ", ";
        killStream << player->GetName() << ", " << player->GetFaction() << ", " << player->getLevel() << ", " << player->GetMaxHealth() << ", " << player->GetHealth() << ", ";
        killStream << creature->GetName() << ", " << creature->GetFaction() << ", " << creature->GetMaxHealth() << ", ";
        killStream << player->GetZoneId() << ", " << player->GetAreaId() << ", " << player->IsGameMaster();
        killStream << "\n";

        fullStream << killStream.str();
        StringDump();
    }
}
