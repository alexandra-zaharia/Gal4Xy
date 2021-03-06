cmake_minimum_required(VERSION 3.5)
project(Gal4Xy C)

set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic -Wconversion")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(include lib)

add_executable(Gal4Xy
        src/main.c
        src/galaxy.c
        src/sector.c
        src/utils.c
        src/io.c
        src/notifications.c
        src/planet.c
        src/player.c
        src/fleet.c
        src/ai.c
        src/battle.c)

target_link_libraries(Gal4Xy ${CMAKE_SOURCE_DIR}/lib/libgcds.a m)

add_subdirectory(tests)
