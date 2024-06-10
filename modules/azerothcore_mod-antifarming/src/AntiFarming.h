#ifndef ANTIFARMING_H
#define ANTIFARMING_H

#include "ScriptMgr.h"
#include "Player.h"
#include "BanMgr.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "cs_antifarming.h"
#include "Configuration/Config.h"

class AntiFarming {
public:
    static AntiFarming* instance();
    typedef std::map<uint64, uint8> antiFarmingData;
    antiFarmingData dataMap;
};
#define sAntiFarming AntiFarming::instance()

#endif