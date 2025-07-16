cmake_minimum_required(VERSION 3.19)
project(
        sts_lightspeed
        VERSION 0.1
        DESCRIPTION "High Performance, rng accurate implementation of Slay the Spire with tools for tree search and ml."
        LANGUAGES CXX)

file (GLOB_RECURSE sts_lightspeed_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Create a filtered source list for battle executable (exclude conflicting files)
set(sts_battle_SOURCES ${sts_lightspeed_SOURCES})
list(REMOVE_ITEM sts_battle_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/combat/BattleContext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameContext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/combat/CardManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Game.cpp"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "-Wno-shift-count-overflow -O3")

add_subdirectory(json)

# target_include_directories(slaythespire PUBLIC include)
# target_include_directories(slaythespire PUBLIC json/single_include)
# target_include_directories(slaythespire PUBLIC bindings)


add_executable(main apps/main.cpp ${sts_lightspeed_SOURCES})
target_link_directories(main PRIVATE json::nlohmann_json)
target_include_directories(main PUBLIC include)
target_include_directories(main PUBLIC json/include)

add_executable(test apps/test.cpp ${sts_lightspeed_SOURCES})
target_include_directories(test PUBLIC include)
target_link_directories(test PUBLIC json::nlohmann_json)
target_include_directories(test PUBLIC json/include)

add_executable(small-test apps/small-test.cpp ${sts_lightspeed_SOURCES})
target_link_directories(small-test PRIVATE json::nlohmann_json)
target_include_directories(small-test PUBLIC include)
target_include_directories(small-test PUBLIC json/include)

add_executable(battle battle/main.cpp battle/FightDefinition.cpp battle/BattleContext2.cpp battle/GameContext2.cpp battle/CardManager2.cpp battle/Game2.cpp ${sts_battle_SOURCES})
target_link_directories(battle PRIVATE json::nlohmann_json)
target_include_directories(battle PUBLIC include)
target_include_directories(battle PUBLIC json/include)
target_include_directories(battle PUBLIC battle)
