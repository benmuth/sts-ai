#include <iostream>

#include "combat/BattleContext.h"
#include "constants/CardPools.h"
#include "constants/MonsterEncounters.h"
#include "data_structure/fixed_list.h"
#include "game/Game.h"
#include "game/GameContext.h"
#include "game/Map.h"
#include "game/Neow.h"
#include "sim/ConsoleSimulator.h"
#include "sim/search/SimpleAgent.h"

using namespace sts;

int main() {
  // while (!std::cin.eof()) {
  //     std::cout << "enter the following on a line: seed character(I/S/D/W)
  //     ascensionLevel" << std::endl;

  //     SimulatorContext simCtx;
  //     ConsoleSimulator simulator;
  //     simulator.play(std::cin, std::cout, simCtx);
  // }

  // sts::search::myGetBestCardToPlay();
  //

  int seed = 1984;
  GameContext gc(CharacterClass::IRONCLAD, seed, 0);

  MonsterEncounter monster = MonsterEncounter::JAW_WORM;

  BattleContext bc;
  bc.init(gc, monster);

  return 0;
}
