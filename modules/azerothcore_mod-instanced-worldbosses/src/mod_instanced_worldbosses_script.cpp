/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Corpse.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "Tokenize.h"
#include "WorldBosses.h"

enum Settings
{
    SETTING_BOSS_TIME,
    SETTING_BOSS_STATUS,
    SETTING_ALLOW_LOOT
};

enum Phases
{
    PHASE_NORMAL = 1,
    PHASE_OUTRO  = 2
};

enum SaveStatus
{
    STATUS_UNLOCKED  = 0,
    STATUS_SAVED     = 1
};

struct SaveData
{
    uint32 creatureId;
    ObjectGuid _ownerGuid;
    GuidSet groupPlayerGUIDS;
};

WorldBosses* WorldBosses::instance()
{
    static WorldBosses instance;
    return &instance;
}

bool WorldBosses::IsWorldBoss(uint32 entry)
{
    switch (entry)
    {
        case 6109: // Azuregos
        case 12397: // Lord Kazzak
        case 14887: // Ysondre
        case 14888: // Lethon
        case 14889: // Emeriss
        case 14890: // Taerar
        case 17711: // Doomwalker
        case 18728: // Doom Lord Kazzak
            return true;
    }

    return false;
}

bool WorldBosses::IsPlayerSaved(Player* player, uint32 entry)
{
    return player->GetPlayerSetting(sWorldBosses->GetSettingSourceStr(entry), SETTING_BOSS_STATUS).value;
}

void WorldBosses::SetSaveStatus(Player* player, uint32 entry, uint8 status)
{
    player->UpdatePlayerSetting(sWorldBosses->GetSettingSourceStr(entry), SETTING_BOSS_TIME, status == STATUS_SAVED ? time(nullptr) : 0);
    player->UpdatePlayerSetting(sWorldBosses->GetSettingSourceStr(entry), SETTING_BOSS_STATUS, status);

    if (CreatureTemplate const* creature = sObjectMgr->GetCreatureTemplate(entry))
    {
        if (status == STATUS_UNLOCKED)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Your lock for {} has reset.", creature->Name);
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("You are now locked to this boss ({}) and may not receive loot until the lock expires.", creature->Name);
        }
    }
}

class mod_instanced_worldbosses_worldscript : public WorldScript
{
public:
    mod_instanced_worldbosses_worldscript() : WorldScript("mod_instanced_worldbosses_worldscript", {
        WORLDHOOK_ON_AFTER_CONFIG_LOAD
    }) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sWorldBosses->IsEnabled = sConfigMgr->GetOption<bool>("ModInstancedWorldBosses.Enable", false);
        sWorldBosses->IsPhasingEnabled = sConfigMgr->GetOption<bool>("ModInstancedWorldBosses.PhaseBosses", false);
        sWorldBosses->IsTuningEnabled = sConfigMgr->GetOption<bool>("ModInstancedWorldBosses.Tuning", false);
        sWorldBosses->BossRespawnTimerSecs = sConfigMgr->GetOption<uint32>("ModInstancedWorldBosses.RespawnTimerSecs", HOUR);
        sWorldBosses->BossLockoutResetSecs = sConfigMgr->GetOption<uint32>("ModInstancedWorldBosses.ResetTimerSecs", 259200);
        sWorldBosses->GracePeriod = sConfigMgr->GetOption<uint32>("ModInstancedWorldBosses.GracePeriodSecs", 300);
    }
};

class GlobalModInstancedBossesScript : public GlobalScript
{
public:
    GlobalModInstancedBossesScript() : GlobalScript("GlobalModInstancedBossesScript", {
        GLOBALHOOK_ON_ALLOWED_FOR_PLAYER_LOOT_CHECK,
        GLOBALHOOK_ON_ALLOWED_TO_LOOT_CONTAINER_CHECK
    }) { }

    bool IsAllowedToLoot(Player const* player, ObjectGuid source)
    {
        if (!sWorldBosses->IsEnabled)
        {
            return false;
        }

        if (source.IsCreature())
        {
            if (Creature* creature = ObjectAccessor::GetCreature(*player, source))
            {
                if (sWorldBosses->IsWorldBoss(creature->GetEntry()))
                {
                    if (Player* looter = ObjectAccessor::FindConnectedPlayer(player->GetGUID()))
                    {
                        uint32 currentTimer = looter->GetPlayerSetting(sWorldBosses->GetSettingSourceStr(source.GetEntry()), SETTING_BOSS_TIME).value;

                        if (!sWorldBosses->IsPlayerSaved(looter, source.GetEntry()))
                        {
                            return false;
                        }

                        if (time(nullptr) >= (currentTimer + sWorldBosses->BossLockoutResetSecs))
                        {
                            sWorldBosses->SetSaveStatus(looter, source.GetEntry(), STATUS_SAVED);
                            return false;
                        }

                        return time(nullptr) >= (currentTimer + sWorldBosses->GracePeriod);
                    }
                }
            }
        }

        return false;
    }

    // Called when the item pops (e.g clicked mob, group loot, master loot list. etc.)
    bool OnAllowedForPlayerLootCheck(Player const* player, ObjectGuid source) override
    {
        return IsAllowedToLoot(player, source);
    }

    // Called when trying to click corpse
    bool OnAllowedToLootContainerCheck(Player const* player, ObjectGuid source) override
    {
        return IsAllowedToLoot(player, source);
    }
};

class ModInstancedBossesPlayerScript : public PlayerScript
{
public:
    ModInstancedBossesPlayerScript() : PlayerScript("ModInstancedBossesPlayerScript", {
        PLAYERHOOK_ON_LOGIN
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (!sWorldBosses->IsEnabled)
        {
            return;
        }

        std::vector<uint32> bossIds = { 6109, 12397, 14887, 14888, 14889, 14890, 17711, 18728 };

        for (auto token : bossIds)
        {
            if (uint32 currentTimer = player->GetPlayerSetting(ModInstancedBosses + Acore::ToString(token), SETTING_BOSS_TIME).value)
            {
                if (time(nullptr) >= (currentTimer + sWorldBosses->BossLockoutResetSecs))
                {
                    sWorldBosses->SetSaveStatus(player, token, STATUS_UNLOCKED);
                }
            }
        }
    }
};

class unit_worldbosses_script : public UnitScript
{
public:
    unit_worldbosses_script() : UnitScript("unit_worldbosses_script", true, {
        UNITHOOK_ON_UNIT_ENTER_COMBAT,
        UNITHOOK_ON_UNIT_ENTER_EVADE_MODE,
        UNITHOOK_ON_UNIT_DEATH
    }) { }

    void OnUnitEnterCombat(Unit* me, Unit* enemy) override
    {
        if (!sWorldBosses->IsEnabled)
        {
            return;
        }

        if (me->ToCreature() && sWorldBosses->IsWorldBoss(me->GetEntry()))
        {
            Player* player = enemy->ToPlayer();

            if (!player)
            {
                player = enemy->GetCharmerOrOwnerPlayerOrPlayerItself();
            }

            if (!player)
            {
                return;
            }

            if (sWorldBosses->IsPhasingEnabled)
            {
                me->SetPhaseMask(PHASE_OUTRO, true);
                PhaseOutPlayers(player, PHASE_OUTRO, me->ToCreature());
            }

            SaveData data;
            data.creatureId = me->GetEntry();
            data._ownerGuid = player->GetGUID();
            saveData[me->GetEntry()] = data;
        }
    }

    void OnUnitEnterEvadeMode(Unit* me, uint8 /*evadeReason*/) override
    {
        if (!sWorldBosses->IsEnabled)
        {
            return;
        }

        if (sWorldBosses->IsWorldBoss(me->GetEntry()))
        {
            if (sWorldBosses->IsPhasingEnabled)
            {
                if (Player* player = ObjectAccessor::FindConnectedPlayer(saveData[me->GetEntry()]._ownerGuid))
                    PhaseOutPlayers(player, PHASE_NORMAL, me->ToCreature());
            }

            me->SetPhaseMask(PHASE_NORMAL, true);

            saveData.erase(me->GetEntry());
        }
    }

    void OnUnitDeath(Unit* me, Unit* /*killer*/) override
    {
        if (!sWorldBosses->IsEnabled)
        {
            return;
        }

        if (sWorldBosses->IsWorldBoss(me->GetEntry()))
        {
            me->ToCreature()->SetRespawnTime(sWorldBosses->BossRespawnTimerSecs);
            me->SaveRespawnTime();

            ObjectGuid playerGUID = saveData[me->GetEntry()]._ownerGuid;

            if (sWorldBosses->IsPhasingEnabled)
            {
                if (Player* player = ObjectAccessor::FindConnectedPlayer(playerGUID))
                    PhaseOutPlayers(player, PHASE_NORMAL, me->ToCreature());
            }

            me->SetPhaseMask(PHASE_NORMAL, true);
            if (Player* player = ObjectAccessor::FindConnectedPlayer(playerGUID))
                LockPlayers(player, me->ToCreature());

            saveData.erase(me->GetEntry());
        }
    }

    void LockPlayers(Player* source, Creature* me)
    {
        if (Group* group = source->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* groupGuy = itr->GetSource();

                if (!groupGuy)
                {
                    continue;
                }

                if (!groupGuy->IsInWorld())
                {
                    continue;
                }

                if (!groupGuy->IsWithinDist(me, 500.0f))
                {
                    continue;
                }

                if (uint32 currentTimer = groupGuy->GetPlayerSetting(sWorldBosses->GetSettingSourceStr(me->GetEntry()), SETTING_BOSS_TIME).value)
                {
                    if (time(nullptr) >= (currentTimer + sWorldBosses->BossLockoutResetSecs))
                    {
                        sWorldBosses->SetSaveStatus(groupGuy, me->GetEntry(), STATUS_UNLOCKED);
                    }
                }

                if (!sWorldBosses->IsPlayerSaved(groupGuy, me->GetEntry()))
                {
                    sWorldBosses->SetSaveStatus(groupGuy, me->GetEntry(), STATUS_SAVED);
                }
            }
        }
        else
        {
            if (uint32 currentTimer = source->GetPlayerSetting(ModInstancedBosses + Acore::ToString(me->GetEntry()), SETTING_BOSS_TIME).value)
            {
                if (time(nullptr) >= (currentTimer + sWorldBosses->BossLockoutResetSecs))
                {
                    sWorldBosses->SetSaveStatus(source, me->GetEntry(), STATUS_UNLOCKED);

                }
            }

            if (!sWorldBosses->IsPlayerSaved(source, me->GetEntry()))
            {
                sWorldBosses->SetSaveStatus(source, me->GetEntry(), STATUS_SAVED);
            }
        }
    }

    void PhaseOutPlayers(Player* source, uint8 phase, Creature* me)
    {
        GuidSet groupPlayerGUIDS;

        if (Group* group = source->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* groupGuy = itr->GetSource();

                if (!groupGuy)
                {
                    continue;
                }

                saveData[me->GetEntry()].groupPlayerGUIDS.insert(groupGuy->GetGUID());

                if (!groupGuy->IsInWorld())
                {
                    continue;
                }

                if (!groupGuy->IsWithinDist(me, 500.0f))
                {
                    continue;
                }

                if (sConfigMgr->GetOption<bool>("ModInstancedWorldBosses.SkipSavedPlayersWhenPhasing", 0))
                {
                    if (sWorldBosses->IsPlayerSaved(groupGuy, me->GetEntry()) && phase == PHASE_OUTRO)
                    {
                        if (groupGuy->GetSession())
                        {
                            ChatHandler(groupGuy->GetSession()).PSendSysMessage("Phasing failed: you are already saved to this boss ({}).", me->GetNameForLocaleIdx(groupGuy->GetSession()->GetSessionDbLocaleIndex()));
                        }

                        continue;
                    }
                }

                groupGuy->SetPhaseMask(phase, true);

                if (!groupGuy->IsAlive() && phase == PHASE_NORMAL)
                {
                    if (Corpse* corpse = groupGuy->GetCorpse())
                    {
                        corpse->SetPhaseMask(PHASE_NORMAL, true);
                    }
                }

                if (sWorldBosses->IsTuningEnabled)
                {
                    HandleDebuffs(groupGuy, phase);
                }
            }
        }
        else
        {
            source->SetPhaseMask(phase, true);

            if (!source->IsAlive() && phase == PHASE_NORMAL)
            {
                if (Corpse* corpse = source->GetCorpse())
                {
                    corpse->SetPhaseMask(PHASE_NORMAL, true);
                }
            }

            if (sWorldBosses->IsTuningEnabled)
            {
                HandleDebuffs(source, phase);
            }
        }

        if (sWorldBosses->IsTuningEnabled)
        {
            for (ObjectGuid guid : saveData[me->GetEntry()].groupPlayerGUIDS)
            {
                if (Player* player = ObjectAccessor::FindConnectedPlayer(guid))
                {
                    if (sWorldBosses->IsPlayerSaved(player, me->GetEntry()))
                    {
                        me->RemoveAllowedLooter(player->GetGUID());
                    }

                    HandleDebuffs(player, phase);
                }
            }
        }
    }

    void HandleDebuffs(Player* player, uint8 phase)
    {
        std::string items = sConfigMgr->GetOption<std::string>("ModInstancedWorldBosses.DebuffCastList", "");

        std::vector<std::string_view> tokens = Acore::Tokenize(items, ' ', false);

        for (auto token : tokens)
        {
            if (token.empty())
            {
                continue;
            }

            std::vector<std::string_view> itemData = Acore::Tokenize(token, ':', false);

            uint32 spellId = *Acore::StringTo<uint32>(itemData.at(0));
            int32 pct = *Acore::StringTo<int32>(itemData.at(1));

            if (phase == PHASE_OUTRO)
            {
                // The auras can stack...
                if (!player->HasAura(spellId))
                {
                    player->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, pct, player);
                }
            }
            else
            {
                player->RemoveAura(*Acore::StringTo<uint32>(itemData.at(0)));
            }
        }
    }

    private:
        std::map<uint32, SaveData> saveData;
};


void Addmod_instanced_worldbosses_script()
{
    new GlobalModInstancedBossesScript();
    new ModInstancedBossesPlayerScript();
    new unit_worldbosses_script();
    new mod_instanced_worldbosses_worldscript();
}
