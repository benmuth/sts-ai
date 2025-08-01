//
// Created by keega on 9/27/2021.
//

#ifndef STS_LIGHTSPEED_SIMPLEAGENT_H
#define STS_LIGHTSPEED_SIMPLEAGENT_H

#include "GameContext2.h"
#include "Action2.h"
#include "sim/search/GameAction.h"

// Forward declarations to avoid conflicts
namespace sts {
    class BattleContext;
}

namespace sts::search {

     // class SimpleAgent {
     //  public:
     //      virtual ~SimpleAgent() = default;  // Always have virtual destructor

     //      void stepBattleCardPlay(BattleContext &bc);
     //      // Pure virtual functions = interface contract
     //      // virtual void playoutBattle(BattleContext& bc) = 0;

     //      // Optional: concrete methods that use the interface
     //      // void runBattle(const GameContext& gc) {
     //      //     playoutBattle(bc);  // Calls derived implementation
     //      // }
     //  };


    struct SimpleAgent {

        std::vector<int> actionHistory;
        GameContext *curGameContext; // unsafe only use in private methods during playout

        fixed_list<int,16> mapPath;

        bool print = false;

        SimpleAgent();

        [[nodiscard]] int getIncomingDamage(const BattleContext &bc) const;

        // void playout(GameContext &gc);

        // void takeAction(GameContext &gc, GameAction a);
        void takeAction(BattleContext &bc, Action a);
        void playoutBattle(BattleContext &bc);

        virtual void stepBattleCardPlay(BattleContext &bc);
        void stepBattleCardSelect(BattleContext &bc);

        // void stepOutOfCombat(GameContext &gc);
        // void stepEventScreen(sts::GameContext &gc);
        // void stepRestScreen(GameContext &gc);
        // void stepRewardsScreen(GameContext &gc);
        // void stepCardReward(GameContext &gc);
        // void stepShopScreen(GameContext &gc);

        bool playPotion(BattleContext &bc);
        // static fixed_list<int,16> getBestMapPathForWeights(const Map &m, const int *weights);
        static void runAgentsMt(int threadCount, std::uint64_t startSeed, int playoutCount, bool print);
    };

}

#endif //STS_LIGHTSPEED_SIMPLEAGENT_H
