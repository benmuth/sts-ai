#include <iostream>
#include <string>
#include <filesystem>

#include "../battle/BattleContext2.h"
#include "../battle/GameContext2.h"
#include "../battle/SimpleAgent2.h"
#include "../include/constants/MonsterEncounters.h"
#include "../include/utils/scenarios.h"

using namespace sts;

void runAgentOnScenario(const GameContext& gc, bool printDetails = true, bool generateSnapshot = false, const std::string& snapshotDir = "") {
    std::cout << "Running agent on scenario with seed: " << gc.seed << std::endl;

    // Initialize battle context with the scenario's encounter
    BattleContext initialBc;
    initialBc.init(gc, gc.info.encounter, false);

    // Copy for snapshot (capture initial state)
    BattleContext finalBc = initialBc;

    // Create and configure the agent
    search::SimpleAgent agent;
    agent.print = printDetails;

    if (printDetails) {
        std::cout << "  Initial State:" << std::endl;
        std::cout << "    Encounter: " << static_cast<int>(gc.info.encounter) << std::endl;
        std::cout << "    Player HP: " << initialBc.player.curHp << "/" << initialBc.player.maxHp << std::endl;
        std::cout << "    Hand size: " << initialBc.cards.cardsInHand << std::endl;
        std::cout << "    Deck size: " << initialBc.cards.drawPile.size() << std::endl;
        std::cout << "  Starting battle..." << std::endl;
    }

    // Run the agent battle simulation
    agent.playoutBattle(finalBc);

    // Report results
    std::cout << "  Battle Result: ";
    switch (finalBc.outcome) {
        case Outcome::PLAYER_VICTORY:
            std::cout << "VICTORY";
            break;
        case Outcome::PLAYER_LOSS:
            std::cout << "DEFEAT";
            break;
        default:
            std::cout << "UNDECIDED";
            break;
    }
    std::cout << " (Final HP: " << finalBc.player.curHp << "/" << finalBc.player.maxHp
                  << ", Turns: " << finalBc.turn << ")" << std::endl;

    // Generate snapshot if requested
    if (generateSnapshot && !snapshotDir.empty()) {
        std::string encounterName = utils::getMonsterName(initialBc.monsters.arr[0].id);
        std::string scenarioName = "agent_" + encounterName + "_" + std::to_string(gc.seed);
        std::string snapshot = utils::formatBattleSnapshot(gc, initialBc, finalBc, scenarioName);

        std::string filename = scenarioName + ".snap";
        std::string filepath = snapshotDir + "/" + filename;

        utils::writeSnapshotToFile(snapshot, filepath);
        std::cout << "  Snapshot written to: " << filepath << std::endl;
    }

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    bool generateSnapshots = false;
    std::string snapshotDir = "snapshots/agent_battles";

    // Check for --snapshot flag
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--snapshot") {
            generateSnapshots = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                snapshotDir = argv[i + 1];
                ++i; // Skip the directory argument
            }
        }
    }

    // Load all scenarios from the scenarios directory
    std::vector<GameContext> scenarios = sts::utils::loadScenariosFromDirectory("tests/scenarios/");

    std::cout << "Loaded " << scenarios.size() << " scenarios" << std::endl;
    if (generateSnapshots) {
        std::cout << "Snapshots will be written to: " << snapshotDir << std::endl;
    }
    std::cout << "========================================" << std::endl;

    // Run SimpleAgent on each scenario
    for (const auto& gc : scenarios) {
        runAgentOnScenario(gc, true, generateSnapshots, snapshotDir);
    }

    std::cout << "All scenarios completed!" << std::endl;
    return 0;
}
