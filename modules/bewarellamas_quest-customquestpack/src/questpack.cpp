/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Creature.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "Mail.h"
#include "AchievementMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Item.h"
#include "MapMgr.h"
#include "WorldSession.h"

uint32 creatureId;
Position* pos;
Map* map;

bool ENABLE_MODULE;
bool Q_POLLUTION;
bool Q_SEWER;

//General NPC scripts
class QuestPack_NPC : public CreatureScript
{
public:
    QuestPack_NPC() : CreatureScript("questpack_npc_script") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {

        if ((creature->GetEntry() == 200003) && (!player->HasSpell(30156)) && player->HasAchieved(953))
        {
            player->PlayerTalkClass->ClearMenus();
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I heard you may have a critter that needs a home?", creature->GetEntry(), 0);
            SendGossipMenuFor(player, 197761, creature->GetGUID());
        }
        else
        {
            SendGossipMenuFor(player, 197761, creature->GetGUID());
        }

        return true;
    }


    bool OnGossipSelect(Player* player, Creature* creature, uint32 Sender, uint32 areaid)
    {
        uint32 creatureID = creature->GetEntry();
        
        switch (creatureID)
        {
        case 200003:
            player->PlayerTalkClass->ClearMenus();
            SendGossipMenuFor(player, 197762, creature->GetGUID());
            player->AddItem(23713, 1);
            break;

        }
        return true;
    }

};


class Questpack_Config : public WorldScript
{
public:
    Questpack_Config() : WorldScript("questpack_config_script") {}

    void OnBeforeConfigLoad(bool) override
    {
        //config reads
        ENABLE_MODULE = sConfigMgr->GetOption<bool>("CQP.Enable", true);
        //Q_POLLUTION = sConfigMgr->GetOption<bool>("CQP.Pollution", true);
        //Q_SEWER = sConfigMgr->GetOption<bool>("CQP.Sewer", true);
        

    }

    void OnStartup() override
    {
        
    }

};

class QuestPack_Player : public PlayerScript
{
public:
    QuestPack_Player() : PlayerScript("questpack_player_script") {}

   
    void OnLogin(Player* player) override
    {
        if (!ENABLE_MODULE)
        {
            return;
        }
        //Cleaning up the Pollution setup 
        if (player->HasAchieved(953) && (player->GetQuestStatus(30001) != QUEST_STATUS_REWARDED) && Q_POLLUTION)
        {

            player->AddQuest(sObjectMgr->GetQuestTemplate(30001), nullptr);
            player->CompleteQuest(30001);
            player->RewardQuest(sObjectMgr->GetQuestTemplate(30001), 0, nullptr, true, false);

            std::string subject;
            std::string body;
            subject = "Help Duridz lol";
            body = "Duridz is need help lol. Find in Zanra... Zangen... Zarish.... uhhh, big mushroom swamp.";

          CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
          MailDraft(subject, body).AddItem(Item::CreateItem(23713, 1, nullptr, false, 0)).SendMailTo(trans, player, MailSender(200003), MAIL_CHECK_MASK_NONE, 0U, 0U, false, true);
          CharacterDatabase.CommitTransaction(trans);
        }

    }


};

// Add all scripts in one
void AddMyScripts()
{
    new Questpack_Config();
    new QuestPack_NPC();
    new QuestPack_Player();
}
