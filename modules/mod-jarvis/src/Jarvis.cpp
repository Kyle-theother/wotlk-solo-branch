#include "Chat.h"
#include "CreatureScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include <string>

enum JarvisAction
{
    ACT_SALE = 1, ACT_PROF = 2, ACT_REP = 3, ACT_RESET = 4, ACT_HEIRLOOM = 5,
    ACT_TRAVEL = 6, ACT_CITIES = 7, ACT_RAIDS = 8, ACT_ZONES = 9,
    ACT_RACIAL = 20, ACT_TALENT = 21, ACT_SPELLS = 22, ACT_BACK = 99
};
