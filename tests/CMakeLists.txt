cmake_minimum_required(VERSION 3.10)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic -Wconversion")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

set(TEST_SOURCES test_galaxy.c)
set(GALAXY_SOURCES
		../src/ai.c
		../src/battle.c
		../src/fleet.c
		../src/galaxy.c
		../src/io.c
		../src/notifications.c
		../src/planet.c
		../src/player.c
		../src/sector.c
		../src/utils.c)

add_executable(test_galaxy ${TEST_SOURCES} ${GALAXY_SOURCES})
target_include_directories(test_galaxy PRIVATE ../src ../include)

find_library(GCDS gcds HINTS ../lib)
target_link_libraries(test_galaxy PUBLIC ${GCDS} cmocka m)
