cmake_minimum_required(VERSION 3.30)
project(Shiny VERSION 1.0
              DESCRIPTION "Swift-like interpreter in C++"
              LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

################
# DEPENDENCIES #
################

include(FetchContent)

FetchContent_Declare(argparse GIT_REPOSITORY https://github.com/p-ranav/argparse.git)
FetchContent_MakeAvailable(argparse)

##############
# EXECUTABLE #
##############

include_directories(${argparse_SOURCE_DIR}/include)
add_executable(
        shiny
        src/main.cpp
        src/scanner.h
        src/parser.h
        src/type.h
        src/ast/ast_node.h
        src/ast/ast_printer.h
)
target_link_libraries(
        shiny
        argparse
)
