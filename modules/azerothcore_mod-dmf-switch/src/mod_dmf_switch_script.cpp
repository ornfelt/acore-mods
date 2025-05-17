/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "loader.h"
#include "ScriptMgr.h"
#include "Config.h"

enum EventIds
{
    EVENT_DMF_TEROKKAR         = 3,
    EVENT_DMF_ELWYNN           = 4,
    EVENT_DMF_MULGORE          = 5
};

class dmf_switch_event_script : public GameEventScript
{
public:
    dmf_switch_event_script() : GameEventScript("dmf_switch_event_script", {
        GAMEEVENTHOOK_ON_START,
        GAMEEVENTHOOK_ON_EVENT_CHECK
    }) { }

    void OnStart(uint16 eventId) override
    {
        if (!sConfigMgr->GetOption<int>("ModDMFSwitch.Enable", 0))
        {
            return;
        }

        switch (eventId)
        {
            case EVENT_DMF_TEROKKAR:
                sGameEventMgr->StopEvent(eventId, true);
                sGameEventMgr->StartEvent(sConfigMgr->GetOption<int>("ModDMFSwitch.SwitchTerokkar", EVENT_DMF_ELWYNN), true);
                break;
        }
    }

    void OnEventCheck(uint16 /*eventId*/) override
    {
        if (!sConfigMgr->GetOption<int>("ModDMFSwitch.Enable", 0))
        {
            return;
        }

        std::vector<uint16> eventIds = { EVENT_DMF_TEROKKAR, EVENT_DMF_MULGORE, EVENT_DMF_ELWYNN };

        for (uint16 const& activeEvent : eventIds)
        {
            if (sGameEventMgr->IsActiveEvent(activeEvent))
            {
                time_t t = time(nullptr);
                tm* now = localtime(&t);

                if (now->tm_wday == 6 /* Saturday */)
                    if (now ->tm_hour == 23) 
                    {
                        GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();

                        if (std::size_t(activeEvent) >= events.size())
                        {
                            LOG_ERROR("module", "[DMF-Switch]: Error, tried to stop unexisting event. ID: {}", activeEvent);
                            return;
                        }

                        GameEventData const& eventData = events[activeEvent];

                        sGameEventMgr->StopEvent(activeEvent, true);
                        LOG_INFO("module", "[DMF-Switch]: Stopping {} ({})", eventData.Description, activeEvent);
                    }
            }
        }
    }
};

void Addmod_dmf_switch_eventScripts()
{
    new dmf_switch_event_script();
}
