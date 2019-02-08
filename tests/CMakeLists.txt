cmake_minimum_required(VERSION 3.10)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic -Wconversion")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

set(TEST_SOURCES test_galaxy.c)
set(GALAXY_SOURCES 
	../ai.c
	../battle.c
	../fleet.c
	../galaxy.c
	../io.c
	../notifications.c
	../planet.c
	../player.c
	../sector.c
	../utils.c)

add_executable(test_galaxy ${TEST_SOURCES} ${GALAXY_SOURCES})
target_include_directories(test_galaxy PRIVATE .. ../include)

find_library(GCDS gcds HINTS ../lib)
target_link_libraries(test_galaxy PUBLIC ${GCDS} cmocka m)
