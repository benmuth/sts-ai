#include <iostream>

#include "combat/BattleContext.h"
#include "constants/CardPools.h"
#include "constants/MonsterEncounters.h"
#include "data_structure/fixed_list.h"
#include "game/GameContext.h"
#include "sim/search/SimpleAgent.h"

using namespace sts;

int main() {
    int seed = 1984;
    GameContext gc(CharacterClass::IRONCLAD, seed, 0);

    MonsterEncounter monster = MonsterEncounter::JAW_WORM;

    BattleContext bc;
    bc.init(gc, monster);

    search::SimpleAgent::myRunAgentMt(1, 1, 1, true);

    return 0;
}