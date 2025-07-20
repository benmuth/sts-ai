cmake_minimum_required(VERSION 3.19)
project(sts_combat_tests)

# Add the parent directory's source files
file (GLOB_RECURSE sts_lightspeed_SOURCES CONFIGURE_DEPENDS "../src/*.cpp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "-Wno-shift-count-overflow -O3")

# Build snapshot generator
add_executable(snapshot_generator utils/snapshot_generator.cpp ${sts_lightspeed_SOURCES})
target_include_directories(snapshot_generator PUBLIC ../include)
target_include_directories(snapshot_generator PUBLIC ../json/include)
