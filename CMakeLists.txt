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

add_executable(
        shiny
        src/vm/vm.cc
        src/main.cc
        src/scanner.h
        src/parser.h
        src/type.h
        src/ast_node.h
        src/ast_printer.h
        src/type_checker.h
)
target_link_libraries(
        shiny
        argparse
)
