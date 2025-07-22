//
// Minimal snapshot generator for combat testing
//

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <optional>

#include "../../include/constants/MonsterEncounters.h"
#include "../../include/game/Card.h"
#include "../../json/single_include/nlohmann/json.hpp"
#include "../../include/sim/search/Action.h"

// #define BATTLE_ONLY false

#ifndef BATTLE_ONLY
    #include "../../battle/GameContext2.h"
    #include "../../battle/BattleContext2.h"
#else
    #include "../../include/game/GameContext.h"
    #include "../../include/combat/BattleContext.h"
#endif

using json = nlohmann::json;
using namespace sts;

class CombatSnapshotGenerator {
private:
    std::stringstream snapshot;
    
public:
    std::string generateSnapshot(const std::string& scenarioPath) {
        // Load scenario from JSON
        std::ifstream file(scenarioPath);
        json scenario;
        file >> scenario;
        
        snapshot.str("");
        snapshot.clear();
        
        // Header
        snapshot << "=== COMBAT: " << scenario["name"] << " ===" << std::endl;
        snapshot << "Seed: " << scenario["seed"] << " | Ascension: " << scenario["ascension"] 
                 << " | Floor: " << scenario["floor"] << std::endl << std::endl;
        
        // Setup game context from scenario
        GameContext gc = createGameContextFromScenario(scenario);
        
        // Initialize battle
        BattleContext bc;
        bc.init(gc);
        
        // Format initial state
        formatInitialState(bc, scenario);
        
        // Execute action sequence
        executeActionSequence(bc, scenario["action_sequence"]);
        
        // Format final result (which is just the initial state for now)
        formatFinalResult(bc);
        return snapshot.str();
    }
    
private:
    GameContext createGameContextFromScenario(const json& scenario) {
        auto seed = scenario["seed"].get<std::uint64_t>();
        auto ascension = scenario["ascension"].get<int>();
        
        GameContext gc(CharacterClass::IRONCLAD, seed, ascension);
        
        // Set player stats
        gc.curHp = scenario["initial_state"]["player_hp"];
        gc.maxHp = scenario["initial_state"]["player_max_hp"];
        
        // Set encounter
        std::string encounterStr = scenario["initial_state"]["encounter"];

        int index = 0;
        for (const char* name : monsterEncounterEnumNames) {
            if (name == encounterStr) {
                break;
            }
            ++index;
          }

        if (index >= 0 && index < sizeof(monsterEncounterEnumNames)/sizeof(monsterEncounterEnumNames[0])) {
             gc.info.encounter = MonsterEncounter(index);
        }

        
        // Clear deck and add cards from scenario
        gc.deck.cards.clear();
        for (const auto& cardStr : scenario["initial_state"]["deck"]) {
            if (cardStr == "STRIKE") {
                gc.deck.obtainRaw(Card(CardId::STRIKE_RED));
            } else if (cardStr == "DEFEND") {
                gc.deck.obtainRaw(Card(CardId::DEFEND_RED));
            } else if (cardStr == "BASH") {
                gc.deck.obtainRaw(Card(CardId::BASH));
            }
        }
        
        return gc;
    }
    
    void formatInitialState(const BattleContext& bc, const json& scenario) {
        snapshot << "Initial State:" << std::endl;
        snapshot << "  Player: " << bc.player.curHp << "/" << bc.player.maxHp 
                 << " HP, " << bc.player.energy << " Energy" << std::endl;
        
        // Format monster
        if (bc.monsters.monsterCount > 0) {
            const auto& monster = bc.monsters.arr[0];
            snapshot << "  Enemy: " << getMonsterName(monster.id) 
                     << " (" << monster.curHp << " HP)";
            
            // Add intent if available
            if (monster.moveHistory[0] != MMID::INVALID) {
                snapshot << " - Intent: " << getMoveName(monster.moveHistory[0]);
            }
            snapshot << std::endl;
        }
        
        // Format hand
        snapshot << "  Hand: ";
        for (int i = 0; i < bc.cards.cardsInHand; ++i) {
            if (i > 0) snapshot << ", ";
            const auto& card = bc.cards.hand[i];
            snapshot << getCardName(card.getId()) << "(" << int(card.cost) << ")";
        }
        snapshot << std::endl;
        
        snapshot << "  Deck: " << bc.cards.drawPile.size() << " cards remaining" << std::endl;
        // TODO(ben): add potions to initial state
        snapshot << std::endl;
    }
    
    void executeActionSequence(BattleContext& bc, const json& actions) {
        snapshot << "Combat Progression:" << std::endl;

#ifndef BATTLE_ONLY
    snapshot << "broken";
#endif

        int turnNumber = 1;

        for (const auto& actionStr : actions) {
            std::string action = actionStr.get<std::string>();

            // Parse and execute the action
            auto searchAction = parseAction(action);
            if (searchAction.has_value()) {
                // Validate action before execution
                if (searchAction->isValidAction(bc)) {
                    snapshot << "  Turn " << turnNumber << ": " << action;

                    // Execute the action
                    searchAction->execute(bc);

                    // Format the result
                    formatActionResult(bc, action);

                    if (action == "end_turn") {
                        turnNumber++;
                    }

                } else {
                    snapshot << "  Invalid action: " << action << " (skipped)" << std::endl;
                }
            } else {
                snapshot << "  Unknown action: " << action << " (skipped)" << std::endl;
            }

            // Check if combat is over
            if (bc.outcome != Outcome::UNDECIDED) {
                snapshot << "  Combat ended!" << std::endl;
                break;
            }
        }

        snapshot << std::endl;
    }
    
    void formatMonsterTurn(const BattleContext& bc) {
        if (bc.monsters.monsterCount > 0) {
            const auto& monster = bc.monsters.arr[0];
            if (monster.isAlive()) {
                snapshot << "  " << getMonsterName(monster.id) 
                         << " attacks → Player (6 damage)" << std::endl; // Simplified
                snapshot << "  Player: " << bc.player.curHp << "/" << bc.player.maxHp 
                         << " HP" << std::endl;
            }
        }
        snapshot << std::endl;
    }
    
    void formatFinalResult(const BattleContext& bc) {
        snapshot << "Final Result:" << std::endl;
        snapshot << "  Outcome: ";
        switch (bc.outcome) {
            case Outcome::PLAYER_VICTORY:
                snapshot << "PLAYER_VICTORY" << std::endl;
                break;
            case Outcome::PLAYER_LOSS:
                snapshot << "PLAYER_LOSS" << std::endl;
                break;
            default:
                snapshot << "UNDECIDED" << std::endl;
                break;
        }
        
        // TODO(ben): add potions to final snapshot result
        snapshot << "  Player HP: " << bc.player.curHp << "/" << bc.player.maxHp << std::endl;
        snapshot << "  Turns: " << bc.turn << std::endl;
        
        // RNG counters for determinism verification
        snapshot << "  RNG Counters: shuffle=" << bc.shuffleRng.counter 
                 << ", cardRandom=" << bc.cardRandomRng.counter 
                 << ", misc=" << bc.miscRng.counter << std::endl;
    }
    
    // Action parsing helper
    std::optional<search::Action> parseAction(const std::string& actionStr) {
        if (actionStr == "end_turn") {
            return search::Action(search::ActionType::END_TURN);
        }
        else if (actionStr.substr(0, 10) == "play_card_") {
            try {
                int cardIndex = std::stoi(actionStr.substr(10));
                // For simplicity, assume target 0 (enemy 0 or no target needed)
                // More sophisticated targeting would require scenario specification
                return search::Action(search::ActionType::CARD, cardIndex, 0);
            } catch (const std::exception&) {
                return std::nullopt;
            }
        }
        else if (actionStr.substr(0, 13) == "use_potion_") {
            try {
                int potionIndex = std::stoi(actionStr.substr(13));
                return search::Action(search::ActionType::POTION, potionIndex, 0);
            } catch (const std::exception&) {
                return std::nullopt;
            }
        }

        return std::nullopt;
    }

    void formatActionResult(const BattleContext& bc, const std::string& action) {
        if (action == "end_turn") {
            snapshot << " → Turn ended";
            if (bc.monsters.monsterCount > 0 && bc.monsters.arr[0].isAlive()) {
                snapshot << ", enemy acts";
            }
            snapshot << std::endl;

            // Show current state after turn
            snapshot << "    Player: " << bc.player.curHp << "/" << bc.player.maxHp << " HP";
            if (bc.monsters.monsterCount > 0) {
                snapshot << " | Enemy: " << bc.monsters.arr[0].curHp << " HP";
            }
            snapshot << std::endl;
        }
        else if (action.substr(0, 10) == "play_card_") {
            int cardIndex = std::stoi(action.substr(10));
            if (cardIndex >= 0 && cardIndex < bc.cards.hand.size()) {
                auto cardId = bc.cards.hand[cardIndex].getId();
                snapshot << " → Played " << getCardName(cardId);

                // Show damage/effects if applicable
                if (isAttackCard(cardId)) {
                    snapshot << " (damage dealt)";
                } else if (isDefendCard(cardId)) {
                    snapshot << " (gained block)";
                }
                snapshot << std::endl;

                // Show updated state
                snapshot << "    Player: " << bc.player.curHp << "/" << bc.player.maxHp
                         << " HP, " << bc.player.block << " Block, " << bc.player.energy << " Energy";
                if (bc.monsters.monsterCount > 0) {
                    snapshot << " | Enemy: " << bc.monsters.arr[0].curHp << " HP";
                }
                snapshot << std::endl;
            }
        }
        else if (action.substr(0, 13) == "use_potion_") {
            snapshot << " → Used potion" << std::endl;
        }
    }

    // Helper methods for formatting
    bool isAttackCard(CardId id) {
        return id == CardId::STRIKE_RED || id == CardId::BASH ||
               id == CardId::STRIKE_GREEN || id == CardId::STRIKE_BLUE || id == CardId::STRIKE_PURPLE;
    }

    bool isDefendCard(CardId id) {
        return id == CardId::DEFEND_RED || id == CardId::DEFEND_GREEN ||
               id == CardId::DEFEND_BLUE || id == CardId::DEFEND_PURPLE;
    }

    std::string getCardName(CardId id) {
        switch (id) {
            case CardId::STRIKE_RED: return "Strike";
            case CardId::DEFEND_RED: return "Defend";
            case CardId::BASH: return "Bash";
            default: return "Unknown";
        }
    }
    
    std::string getMonsterName(MonsterId id) {
        switch (id) {
            case MonsterId::CULTIST: return "Cultist";
            default: return "Unknown";
        }
    }
    
    std::string getMoveName(MonsterMoveId move) {
        return "Attack"; // Simplified for now
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <scenario.json> <output.snap>" << std::endl;
        return 1;
    }
    
    CombatSnapshotGenerator generator;
    
    try {
        std::string snapshot = generator.generateSnapshot(argv[1]);
        
        std::ofstream outFile(argv[2]);
        outFile << snapshot;
        outFile.close();
        
        std::cout << "Snapshot generated successfully: " << argv[2] << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating snapshot: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
