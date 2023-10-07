cmake_minimum_required(VERSION 3.8)
project(uasm)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

add_executable(uasm
  uasm.c
)

