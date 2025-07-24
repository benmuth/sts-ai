#include <iostream>

#include "../battle/BattleContext2.h"
#include "../battle/GameContext2.h"
#include "../battle/SimpleAgent2.h"
#include "../include/constants/MonsterEncounters.h"

using namespace sts;

int main() {
    // Replicate the functionality from apps/main.cpp but using battle/ directory equivalents
    
    int seed = 1984;
    GameContext gc(CharacterClass::IRONCLAD, seed, 0);

    MonsterEncounter monster = MonsterEncounter::JAW_WORM;

    BattleContext bc;
    bc.init(gc, monster, false);

    // Use the battle-only SimpleAgent equivalent
    search::SimpleAgent::myRunAgentMt(1, 1, 1, true);

    return 0;
}