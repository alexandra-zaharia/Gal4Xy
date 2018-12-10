cmake_minimum_required(VERSION 3.13)
project(Gal4Xy C)

set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

#set(SOURCES main.c galaxy.c lib/vector.h)
include_directories(lib)
#link_directories(lib)

add_executable(Gal4Xy main.c galaxy.h galaxy.c galaxy.c lib/vector.c io.h io.c)
#add_executable(Gal4Xy ${SOURCES})