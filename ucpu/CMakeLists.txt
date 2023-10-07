cmake_minimum_required(VERSION 3.8)

#set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_VERBOSE_MAKEFILE OFF)

project(ucpu
  VERSION 0.1
  DESCRIPTION "uCPU Emulator"
  HOMEPAGE_URL "https://www.murphysys.com"
  LANGUAGES C
)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

add_executable(ucpu
  ucpu.c
  urom.c
  optable.c
  gfx.c
  sdl_event.c
)

list(APPEND CMAKE_MODULE_PATH cmake)

find_package(SDL2 REQUIRED)
add_library(SDL2_BUNDLE INTERFACE)
add_library(SDL2::SDL2 ALIAS SDL2_BUNDLE)
target_link_libraries(ucpu
  PUBLIC SDL2::SDL2
)

target_link_libraries(ucpu INTERFACE
  SDL2::Core
  SDL2::Main
)
