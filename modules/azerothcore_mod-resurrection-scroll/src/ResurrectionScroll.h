#ifndef DEF_RESSURRECTIONSCROLL_H
#define DEF_RESSURRECTIONSCROLL_H

#include "Config.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

enum RSSettings
{
    SETTING_RS_ELIGIBLE      = 0,
    SETTING_RS_DISABLE_DATE  = 1
};

const std::string ModResScrollString = "mod_resurrection_scroll";

class ResurrectionScroll
{
public:
    static ResurrectionScroll* instance();

    bool IsEnabled{ false };
    uint32 DaysInactive{ 180 };
    uint32 Duration{ 30 };
};

#define sResScroll ResurrectionScroll::instance()

#endif
