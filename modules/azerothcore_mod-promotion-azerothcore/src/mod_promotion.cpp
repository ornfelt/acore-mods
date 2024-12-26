/**
 * This plugin can be used for common player customizations
*/

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "GossipDef.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Language.h"
#include "DatabaseEnv.h"
#include "WorldSession.h"
#include "mod_promotion.h"
#include "World.h"

static bool promotionEnable, mountEnable, bagEnable, equippedbags;
static int promotionCount, moneyRewardConst, mountPromotion, bagReward;
static int classConfArmor, LevelForPromotion;

class announce_module : public PlayerScript
{
public:
    announce_module() : PlayerScript("announce_module") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("announce_module.enableHelloWorld", true))
        {
            ChatHandler(player->GetSession()).SendSysMessage("Hello World from Promotion-Module! - By Asmadeuxx");
        }
        else
        {
            ChatHandler(player->GetSession()).SendSysMessage("Hello World from Promotion-Module! - By Asmadeuxx");
        }
    }
};

class npc_promotion : public CreatureScript
{
public:
    npc_promotion() : CreatureScript("npc_promotion") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->getClass() == CLASS_DEATH_KNIGHT)
        {
            return true;
        }

        if (player->getLevel() != 1)
        {
            if (player && creature)
            {
                SendGossipMenuFor(player, 68, creature);
            }
            return true;
        }

        switch (player->getClass())
        {
        case CLASS_WARRIOR:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Dps", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Tank", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            break;
        case CLASS_WARLOCK:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Dps", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            break;
        case CLASS_DRUID:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Tank", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Heal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Balance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            break;
        case CLASS_HUNTER:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Dps ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            break;
        case CLASS_MAGE:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Dps", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            break;
        case CLASS_ROGUE:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Dps", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            break;
        case CLASS_PRIEST:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Heal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Shadow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            break;
        case CLASS_PALADIN:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Tank", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Heal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Dps", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            break;
        case CLASS_SHAMAN:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Elemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "PvP Enhancement", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
            break;
        default:
            break;
        }
        SendGossipMenuFor(player, 68, creature);
        return true;
    }

    void MoneyReward(Player* player)
    {
        player->ModifyMoney(sConfigMgr->GetOption<uint32>("MoneyRewardValue", 25000000));
    }

    void WarriorPromotionDps(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_HEAD", 41350)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_SHOULDERS", 41351)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_CHEST", 41353)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_WAIST", 41352)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_LEGS", 41347)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_FEET", 41348)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_WRISTS", 41354)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_HANDS", 41349)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_TRINKET1", 37064)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_MAINHAND", 37108)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_OFFHAND", 36962)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_RANGED", 36981)), true);
    }

    void WarriorPromotionTank(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_HEAD", 41350)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_SHOULDERS", 41351)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_CHEST", 41353)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_WAIST", 41352)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_LEGS", 41347)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_FEET", 41348)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_WRISTS", 41354)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_HANDS", 41349)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_TRINKET1", 36993)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_MAINHAND", 36984)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_OFFHAND", 37107)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_RANGED", 36981)), true);
    }

    void PaladinPromotionTank(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_HEAD", 41350)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_SHOULDERS", 41351)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_CHEST", 41353)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_WAIST", 41352)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_LEGS", 41347)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_FEET", 41348)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_WRISTS", 41354)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_HANDS", 41349)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_TRINKET1", 36993)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_MAINHAND", 36984)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_OFFHAND", 37107)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_RANGED", 38363)), true);
    }

    void PaladinPromotionHeal(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_HEAD", 42728)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_SHOULDERS", 42727)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_CHEST", 42725)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_WAIST", 42729)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_LEGS", 42726)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_FEET", 42730)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_WRISTS", 42723)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_HANDS", 42724)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_TRINKET1", 37111)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_TRINKET2", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_MAINHAND", 37060)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_OFFHAND", 37061)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_RANGED", 38364)), true);
    }

    void PaladinPromotionDps(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_HEAD", 41350)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_SHOULDERS", 41351)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_CHEST", 41353)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_WAIST", 41352)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_LEGS", 41347)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_FEET", 41348)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_WRISTS", 41354)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_HANDS", 41349)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_TRINKET1", 37064)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_MAINHAND", 37108)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_RANGED", 38362)), true);
    }

    void HunterPromotion(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_HEAD", 43447)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_SHOULDERS", 43449)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_CHEST", 43445)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_WAIST", 43442)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_LEGS", 43448)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_FEET", 43443)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_WRISTS", 43444)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_HANDS", 43446)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_TRINKET1", 37064)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_MAINHAND", 36980)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_RANGED", 37050)), true);
    }

    void RoguePromotion(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_HEAD", 43260)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_SHOULDERS", 43433)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_CHEST", 43434)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_WAIST", 43437)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_LEGS", 43438)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_FEET", 43439)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_WRISTS", 43435)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_HANDS", 43436)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_TRINKET1", 37064)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_MAINHAND", 36944)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_OFFHAND", 37037)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_RANGED", 36981)), true);
    }

    void PriestPromotionHeal(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_HEAD", 43971)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_SHOULDERS", 43973)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_CHEST", 43972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_WAIST", 43969)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_LEGS", 43975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_FEET", 43970)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_WRISTS", 43974)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_HANDS", 41516)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_TRINKET2", 37111)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_MAINHAND", 36975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_RANGED", 37038)), true);
    }

    void PriestPromotionShadow(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_HEAD", 43971)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_SHOULDERS", 43973)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_CHEST", 43972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_WAIST", 43969)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_LEGS", 43975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_FEET", 43970)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_WRISTS", 43974)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_HANDS", 41516)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_TRINKET2", 38358)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_MAINHAND", 36975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_RANGED", 37038)), true);

    }

    void ShamanPromotionCaster(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_HEAD", 43455)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_SHOULDERS", 43457)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_CHEST", 43453)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_WAIST", 43450)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_LEGS", 43456)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_FEET", 43451)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_WRISTS", 43452)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_HANDS", 43454)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_TRINKET2", 38358)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_MAINHAND", 37112)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_OFFHAND", 37061)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_RANGED", 38361)), true);
    }

    void ShamanPromotionMelee(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_HEAD", 43447)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_SHOULDERS", 43449)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_CHEST", 43445)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_WAIST", 43442)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_LEGS", 43448)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_FEET", 43443)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_WRISTS", 43444)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_HANDS", 43446)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_TRINKET1", 37064)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_MAINHAND", 35630)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_OFFHAND", 36944)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_RANGED", 38367)), true);
    }

    void ShamanPromotionHeal(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_HEAD", 43455)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_SHOULDERS", 43457)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_CHEST", 43453)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_WAIST", 43450)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_LEGS", 43456)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_FEET", 43451)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_WRISTS", 43452)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_HANDS", 43454)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_TRINKET2", 37111)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_MAINHAND", 37112)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_OFFHAND", 37061)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_RANGED", 38368)), true);
    }

    void MagePromotionDps(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_HEAD", 43971)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_SHOULDERS", 43973)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_CHEST", 43972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_WAIST", 43969)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_LEGS", 43975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_FEET", 43970)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_WRISTS", 43974)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_HANDS", 41516)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_TRINKET2", 38358)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_MAINHAND", 36975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_RANGED", 37038)), true);

    }

    void WarlockPromotionDps(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_HEAD", 43971)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_SHOULDERS", 43973)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_CHEST", 43972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_WAIST", 43969)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_LEGS", 43975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_FEET", 43970)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_WRISTS", 43974)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_HANDS", 41516)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_TRINKET2", 38358)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_MAINHAND", 36975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_RANGED", 37038)), true);
    }

    void DruidPromotionTank(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_HEAD", 43260)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_SHOULDERS", 43433)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_CHEST", 43434)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_WAIST", 43437)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_LEGS", 43438)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_FEET", 43439)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_WRISTS", 43435)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_HANDS", 43436)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_TRINKET1", 37064)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_MAINHAND", 36980)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_RANGED", 38365)), true);
    }

    void DruidPromotionHeal(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_HEAD", 43261)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_SHOULDERS", 43262)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_CHEST", 43263)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_WAIST", 43266)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_LEGS", 43271)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_FEET", 43273)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_WRISTS", 43264)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_HANDS", 43265)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_TRINKET2", 37111)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_MAINHAND", 36975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_RANGED", 38366)), true);
    }

    void DruidPromotionMelee(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_HEAD", 43260)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_NECK", 45812)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_SHOULDERS", 43433)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_CHEST", 43434)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_WAIST", 43437)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_LEGS", 43438)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_FEET", 43439)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_WRISTS", 43435)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_HANDS", 43436)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_FINGER1", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_FINGER2", 45809)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_TRINKET1", 37064)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_TRINKET2", 38359)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_BACK", 45811)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_MAINHAND", 36980)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_RANGED", 38365)), true);
    }

    void DruidPromotionCaster(Player* player)
    {
        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_HEAD", 43261)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_NECK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_NECK", 45813)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_SHOULDERS", 43262)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_CHEST", 43263)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_WAIST", 43266)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_LEGS", 43271)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FEET, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_FEET", 43273)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_WRISTS", 43264)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_HANDS", 43265)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_FINGER1", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_FINGER2", 45808)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_TRINKET1", 36972)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_TRINKET2", 38358)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_BACK, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_BACK", 45810)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_MAINHAND", 36975)), true);
        player->EquipNewItem(EQUIPMENT_SLOT_RANGED, (sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_RANGED", 38360)), true);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        uint32 accountID = player->GetSession()->GetAccountId();
        QueryResult result = CharacterDatabase.Query("SELECT COUNT(`guid`) FROM `characters` WHERE `account`={}", accountID);
        Field* fields = result->Fetch();
        uint32 pjts = fields[0].Get<uint32>();

        ClearGossipMenuFor(player);

        if (promotionEnable && ((int)pjts <= promotionCount))
        {
            if (action > GOSSIP_ACTION_INFO_DEF && action < 1020)
                player->GiveLevel(sConfigMgr->GetOption<uint32>("LevelForPromotion", 80)); // Level

            //Money 2,5k
            MoneyReward(player);

             // Skill Max
            player->UpdateSkillsToMaxSkillsForLevel();

            // Riding
            player->learnSpell(SKILL_RIDING_75);
            player->learnSpell(SKILL_RIDING_100);
            player->learnSpell(SKILL_RIDING_FLYING);
            player->learnSpell(SKILL_RIDING_ARTISING);

            creature->Whisper("You Got Your Promotion!", LANG_UNIVERSAL, player);

            if (mountEnable)
            {
                player->learnSpell(sConfigMgr->GetOption<uint32>("mountPromotion", 42777)); //Swift Spectral Tiger
            }
        
            //Bags
            if (bagEnable)
            {
                if (equippedbags)
                {
                    for (int slot = INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; slot++)
                        if (Item* bag = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                            player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);

                    for (int slot = INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; slot++)
                        player->EquipNewItem(slot, (sConfigMgr->GetOption<uint32>("bagReward.Id", 14156)), true);
                }
                else
                {
                    player->AddItem((sConfigMgr->GetOption<uint32>("bagReward.Id", 14156)), 4);
                }
            }

            switch (action)
            {
                case 1001:
                    WarriorPromotionDps(player);
                    break;
                case 1002: 
                    WarriorPromotionTank(player);
                    break;
                case 1003:
                    WarlockPromotionDps(player);
                    break;
                case 1004:
                    DruidPromotionTank(player);
                    break;
                case 1005:
                    DruidPromotionHeal(player);
                    break;
                case 1006:
                    DruidPromotionCaster(player);
                    break;
                case 1007:
                    HunterPromotion(player);
                    break;
                case 1008:
                    MagePromotionDps(player);
                    break;
                case 1009:
                    RoguePromotion(player);
                    break;
                case 1010:
                    PriestPromotionHeal(player);
                    break;
                case 1011:
                    PriestPromotionShadow(player);
                    break;
                case 1012:
                    PaladinPromotionTank(player);
                    break;
                case 1013:
                    PaladinPromotionHeal(player);
                    break;
                case 1014:
                    PaladinPromotionDps(player);
                    break;
                default:
                    break;
            }
        }
        else
        {
            SendGossipMenuFor(player, 80000, creature);
        }

        return true;
    }
};

class mod_promotion_conf : public WorldScript
{
public:
    mod_promotion_conf() : WorldScript("mod_promotion.conf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload)
        {
            // Config For Module
            promotionEnable = sConfigMgr->GetOption<bool>("promotionEnable.enable", true);
            promotionCount = sConfigMgr->GetOption<uint32>("promotion.count", 1);
            moneyRewardConst = sConfigMgr->GetOption<uint32>("MoneyRewardValue", 25000000);
            LevelForPromotion = sConfigMgr->GetOption<uint32>("LevelForPromotion", 80);
            mountPromotion = sConfigMgr->GetOption<uint32>("mountPromotion", 42277);
            mountEnable = sConfigMgr->GetOption<bool>("mountEnable.enable", true);
            bagEnable = sConfigMgr->GetOption<bool>("bagEnable.enable", true);
            equippedbags = sConfigMgr->GetOption<bool>("equippedbags.enable", true);
            bagReward = sConfigMgr->GetOption<uint32>("bagReward.Id", 14156);

            /*
            * ARMOR CONFIGURATION FOR CLASSES
            */
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_HEAD", 41350);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_SHOULDERS", 41351);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_CHEST", 41353);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_WAIST", 41352);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_LEGS", 41347);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_FEET", 41348);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_WRISTS", 41354);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_HANDS", 41349);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_TRINKET1", 36993);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_MAINHAND", 36984);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_OFFHAND", 37107);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_TANK_RANGED", 36981);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_HEAD", 41350);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_SHOULDERS", 41351);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_CHEST", 41353);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_WAIST", 41352);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_LEGS", 41347);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_FEET", 41348);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_WRISTS", 41354);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_HANDS", 41349);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_TRINKET1", 37064);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_MAINHAND", 37108);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_OFFHAND", 36962);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARRIOR_DPS_RANGED", 36981);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_HEAD", 41350);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_SHOULDERS", 41351);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_CHEST", 41353);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_WAIST", 41352);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_LEGS", 41347);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_FEET", 41348);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_WRISTS", 41354);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_HANDS", 41349);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_TRINKET1", 36993);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_MAINHAND", 36984);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_OFFHAND", 37107);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_TANK_RANGED", 38363);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_HEAD", 42728);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_SHOULDERS", 42727);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_CHEST", 42725);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_WAIST", 42729);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_LEGS", 42726);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_FEET", 42730);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_WRISTS", 42723);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_HANDS", 42724);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_TRINKET1", 37111);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_TRINKET2", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_MAINHAND", 37060);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_OFFHAND", 37061);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_HEAL_RANGED", 38364);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_HEAD", 41350);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_SHOULDERS", 41351);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_CHEST", 41353);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_WAIST", 41352);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_LEGS", 41347);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_FEET", 41348);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_WRISTS", 41354);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_HANDS", 41349);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_TRINKET1", 37064);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_MAINHAND", 37108);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PALADIN_DPS_RANGED", 38362);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_HEAD", 43447);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_SHOULDERS", 43449);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_CHEST", 43445);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_WAIST", 43442);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_LEGS", 43448);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_FEET", 43443);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_WRISTS", 43444);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_HANDS", 43446);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_TRINKET1", 37064);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_MAINHAND", 36980);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_HUNTER_RANGED", 37050);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_HEAD", 43260);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_SHOULDERS", 43433);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_CHEST", 43434);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_WAIST", 43437);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_LEGS", 43438);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_FEET", 43439);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_WRISTS", 43435);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_HANDS", 43436);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_TRINKET1", 37064);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_MAINHAND", 36944);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_OFFHAND", 37037);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_ROGUE_RANGED", 36981);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_HEAD", 43971);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_SHOULDERS", 43973);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_CHEST", 43972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_WAIST", 43969);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_LEGS", 43975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_FEET", 43970);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_WRISTS", 43974);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_HANDS", 41516);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_TRINKET2", 37111);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_MAINHAND", 36975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_HEAL_RANGED", 37038);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_HEAD", 43971);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_SHOULDERS", 43973);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_CHEST", 43972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_WAIST", 43969);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_LEGS", 43975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_FEET", 43970);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_WRISTS", 43974);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_HANDS", 41516);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_TRINKET2", 38358);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_MAINHAND", 36975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_PRIEST_SHADOW_RANGED", 37038);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_HEAD", 43455);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_SHOULDERS", 43457);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_CHEST", 43453);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_WAIST", 43450);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_LEGS", 43456);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_FEET", 43451);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_WRISTS", 43452);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_HANDS", 43454);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_TRINKET2", 38358);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_MAINHAND", 37112);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_OFFHAND", 37061);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_CASTER_RANGED", 38361);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_HEAD", 43447);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_SHOULDERS", 43449);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_CHEST", 43445);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_WAIST", 43442);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_LEGS", 43448);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_FEET", 43443);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_WRISTS", 43444);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_HANDS", 43446);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_TRINKET1", 37064);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_MAINHAND", 35630);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_OFFHAND", 36944);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_MELEE_RANGED", 38367);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_HEAD", 43455);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_SHOULDERS", 43457);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_CHEST", 43453);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_WAIST", 43450);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_LEGS", 43456);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_FEET", 43451);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_WRISTS", 43452);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_HANDS", 43454);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_TRINKET2", 37111);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_MAINHAND", 37112);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_OFFHAND", 37061);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_SHAMAN_HEAL_RANGED", 38368);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_HEAD", 43971);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_SHOULDERS", 43973);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_CHEST", 43972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_WAIST", 43969);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_LEGS", 43975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_FEET", 43970);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_WRISTS", 43974);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_HANDS", 41516);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_TRINKET2", 38358);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_MAINHAND", 36975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_MAGE_RANGED", 37038);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_HEAD", 43971);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_SHOULDERS", 43973);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_CHEST", 43972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_WAIST", 43969);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_LEGS", 43975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_FEET", 43970);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_WRISTS", 43974);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_HANDS", 41516);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_TRINKET2", 38358);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_MAINHAND", 36975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_WARLOCK_RANGED", 37038);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_HEAD", 43260);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_SHOULDERS", 43433);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_CHEST", 43434);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_WAIST", 43437);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_LEGS", 43438);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_FEET", 43439);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_WRISTS", 43435);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_HANDS", 43436);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_TRINKET1", 37064);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_MAINHAND", 36980);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_TANK_RANGED", 38365);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_HEAD", 43261);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_SHOULDERS", 43262);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_CHEST", 43263);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_WAIST", 43266);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_LEGS", 43271);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_FEET", 43273);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_WRISTS", 43264);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_HANDS", 43265);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_TRINKET2", 37111);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_MAINHAND", 36975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_HEAL_RANGED", 38366);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_HEAD", 43260);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_NECK", 45812);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_SHOULDERS", 43433);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_CHEST", 43434);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_WAIST", 43437);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_LEGS", 43438);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_FEET", 43439);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_WRISTS", 43435);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_HANDS", 43436);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_FINGER1", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_FINGER2", 45809);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_TRINKET1", 37064);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_TRINKET2", 38359);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_BACK", 45811);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_MAINHAND", 36980);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_MELEE_RANGED", 38365);

            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_HEAD", 43261);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_NECK", 45813);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_SHOULDERS", 43262);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_CHEST", 43263);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_WAIST", 43266);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_LEGS", 43271);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_FEET", 43273);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_WRISTS", 43264);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_HANDS", 43265);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_FINGER1", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_FINGER2", 45808);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_TRINKET1", 36972);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_TRINKET2", 38358);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_BACK", 45810);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_MAINHAND", 36975);
            classConfArmor = sConfigMgr->GetOption<uint32>("EQUIPMENT_SLOT_DRUID_CASTER_RANGED", 38360);
        }
    }
};

void AddSC_npc_promotion()
{
    new announce_module();
    new npc_promotion();
    new mod_promotion_conf();
}
