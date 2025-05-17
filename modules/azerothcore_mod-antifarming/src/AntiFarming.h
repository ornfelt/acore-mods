#ifndef ANTIFARMING_H
#define ANTIFARMING_H

#include "AccountMgr.h"
#include "BanMgr.h"
#include "Chat.h"
#include "Configuration/Config.h"
#include "cs_antifarming.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "WorldSessionMgr.h"

class AntiFarming
{
public:
    static AntiFarming* instance();
    typedef std::map<uint64, uint8> antiFarmingData;
    antiFarmingData dataMap;
};
#define sAntiFarming AntiFarming::instance()

#endif