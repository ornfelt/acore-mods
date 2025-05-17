#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

#define MSG_GOSSIP_TEXT_GETTING_STARTED "Welcome to AzerothCore morph service!"

#define MSG_ERR_INCOMBAT "You are in combat. To use my services, Leave it.."

#define MSG_GOSSIP_TEXT_MORTH_GNOME_MALE "[Transform] Gnome, male."
#define MSG_GOSSIP_TEXT_MORTH_GNOME_FEMALE "[Transform] Gnome, female."
#define MSG_GOSSIP_TEXT_MORTH_HUMAN_MALE "[Transform] Human, male."
#define MSG_GOSSIP_TEXT_MORTH_HUMAN_FEMALE "[Transform] Human, female."
#define MSG_GOSSIP_TEXT_MORTH_BLOOD_ELF_MALE "[Transform] Blood Elf, male."
#define MSG_GOSSIP_TEXT_MORTH_BLOOD_ELF_FEMALE "[Transform] Blood Elf, female."
#define MSG_GOSSIP_TEXT_MORTH_TAUREN_MALE "[Transform] Tauren, male."
#define MSG_GOSSIP_TEXT_MORTH_TAUREN_FEMALE "[Transform] Tauren, female."

class npc_morph : public CreatureScript
{
public: 
    npc_morph() : CreatureScript("npc_morph") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_GNOME_MALE, GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_GNOME_FEMALE, GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_HUMAN_MALE, GOSSIP_SENDER_MAIN, 4);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_HUMAN_FEMALE, GOSSIP_SENDER_MAIN, 5);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_BLOOD_ELF_MALE, GOSSIP_SENDER_MAIN, 6);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_BLOOD_ELF_FEMALE, GOSSIP_SENDER_MAIN, 7);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_TAUREN_MALE, GOSSIP_SENDER_MAIN, 8);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, MSG_GOSSIP_TEXT_MORTH_TAUREN_FEMALE, GOSSIP_SENDER_MAIN, 9);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (!player->getAttackers().empty())
        {
            creature->Whisper(MSG_ERR_INCOMBAT, LANG_UNIVERSAL, player);
            ClearGossipMenuFor(player);
            return false;
        }

        switch (action)
        {
            case 2:
                player->SetDisplayId(20580);
                ClearGossipMenuFor(player);
            break;
        case 3:
            player->SetDisplayId(20320);
            ClearGossipMenuFor(player);
            break;
        case 4:
            player->SetDisplayId(15833);
            ClearGossipMenuFor(player);
            break;
        case 5:
            player->SetDisplayId(25056);
            ClearGossipMenuFor(player);
            break;
        case 6:
            player->SetDisplayId(20368);
            ClearGossipMenuFor(player);
            break;
        case 7:
            player->SetDisplayId(20370);
            ClearGossipMenuFor(player);
            break;
        case 8:
            player->SetDisplayId(20319);
            ClearGossipMenuFor(player);
            break;
        case 9:
            player->SetDisplayId(20584);
            ClearGossipMenuFor(player);
            break;
        }

        return true;
    }
};

void AddSC_npc_morph()
{
    new npc_morph();
}
