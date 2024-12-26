#include "ScriptMgr.h"
#include "Player.h"
#include "AccountMgr.h"
#include "Common.h"
#include "Configuration/Config.h"

uint32 TimeCheck_AutoRevive = 0;

class AutoRevive_PlayerSC : public PlayerScript
{
public:
    AutoRevive_PlayerSC() : PlayerScript("AutoRevive_PlayerSC") {}

    void OnBeforeUpdate(Player* player, uint32 diff)
    {
        if (!sConfigMgr->GetOption<bool>("AutoRevive.Enable", false))
            return;

        if (AccountMgr::IsPlayerAccount(player->GetSession()->GetSecurity()))
            return;

        uint32 NowHealth = player->GetHealth();
        uint32 TimeCheck = 1000;
        uint32 zoneid_conf = sConfigMgr->GetOption<uint16>("AutoRevive.ZoneID", 0);
        uint32 zoneid = player->GetZoneId();

        if (TimeCheck_AutoRevive < diff)
        {
            if (NowHealth <= 1)
            {
                if (zoneid_conf > 0)
                {
                    if (zoneid_conf == zoneid)
                    {
                        player->ResurrectPlayer(1.0f);
                        player->SaveToDB(false, false);
                    }
                    else
                        return;
                }
                else
                {
                    player->ResurrectPlayer(1.0f);
                    player->SaveToDB(false, false);
                }
            }

            TimeCheck_AutoRevive = TimeCheck;
        }
        else
        TimeCheck_AutoRevive -= diff;
    }
};

void AddAutoReviveScripts()
{
  new AutoRevive_PlayerSC();
}
