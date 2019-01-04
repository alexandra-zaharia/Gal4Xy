cmake_minimum_required(VERSION 3.13)
project(Gal4Xy C)

set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic -Wconversion")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(include lib)

add_executable(Gal4Xy
        main.c
        galaxy.c
        sector.c
        utils.c
        io.c
        notifications.c
        planet.c
        player.c
        fleet.c
        ai.c)

target_link_libraries(Gal4Xy ${CMAKE_SOURCE_DIR}/lib/libgcds.a m)