cmake_minimum_required(VERSION 3.20.0)

# allow finding our custom board
set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(modulink)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources} )
