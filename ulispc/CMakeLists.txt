cmake_minimum_required(VERSION 3.8)

#set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_VERBOSE_MAKEFILE OFF)

project(ulispc
  VERSION 0.1
  DESCRIPTION "uLisp Compiler"
  HOMEPAGE_URL "https://www.murphysys.com"
  LANGUAGES C
)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

add_executable(ulispc
  ulispc.c
  stringutil.c
  tokenizer.c
  parser.c
  compiler.c
)