#ifndef MODULE_PHASEDDUELS_H
#define MODULE_PHASEDDUELS_H

#include "Chat.h"
#include "Configuration/Config.h"
#include "GridNotifiers.h"
#include "Object.h"
#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellMgr.h"

class PhasedDueling : public PlayerScript
{
public:
    PhasedDueling() : PlayerScript("PhasedDueling", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_DUEL_START,
        PLAYERHOOK_ON_DUEL_END
    }) {}

    // Attempt in storing the player phase with spell phases included.
    uint32 getNormalPhase(Player* player) const;

    void OnPlayerLogin(Player* player) override;
    void OnPlayerDuelStart(Player* firstplayer, Player* secondplayer) override;
    void OnPlayerDuelEnd(Player* firstplayer, Player* secondplayer, DuelCompleteType /*type*/) override;
};

void AddPhasedDuelsScripts()
{
    new PhasedDueling();
}

#endif //MODULE_PHASEDDUELS_H
