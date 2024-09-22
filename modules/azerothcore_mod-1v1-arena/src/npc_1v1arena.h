#include "ScriptMgr.h"


class npc_1v1arena : public CreatureScript
{
public:
    npc_1v1arena() : CreatureScript("npc_1v1arena") { }

    bool OnGossipHello(Player* player, Creature* creature) override;
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override;
    bool JoinQueueArena(Player* player, Creature* /* me */, bool isRated);
    bool CreateArenateam(Player* player, Creature* /* me */);
private:
    bool Arena1v1CheckTalents(Player* player);
};
