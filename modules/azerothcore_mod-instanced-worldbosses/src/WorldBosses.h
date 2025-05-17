#ifndef DEF_WORLDBOSSES_H
#define DEF_WORLDBOSSES_H

#include "Player.h"
#include "Config.h"

const std::string ModInstancedBosses = "mod-instanced-worldbosses#";

class WorldBosses
{
public:
    static WorldBosses* instance();

    bool IsEnabled{ false };
    bool IsPhasingEnabled{ false };
    bool IsTuningEnabled{ false };
    uint32 BossRespawnTimerSecs{ HOUR }; // Time until boss respawns in seconds.
    uint32 BossLockoutResetSecs{ 259200 }; // Time until the boss lockouet resets in seconds.
    uint32 GracePeriod{ 300 };
    [[nodiscard]] bool IsWorldBoss(uint32 entry);
    [[nodiscard]] bool IsPlayerSaved(Player* player, uint32 entry);
    [[nodiscard]] std::string GetSettingSourceStr(uint32 entry) { return ModInstancedBosses + Acore::ToString(entry); };
    void SetSaveStatus(Player* player, uint32 entry, uint8 status);
};

#define sWorldBosses WorldBosses::instance()

#endif
