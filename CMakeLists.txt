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
        src/main.cpp
        src/error.h
        src/bytecode.h
        src/frontend/scanner.h
        src/frontend/expr.h
        src/frontend/type.h
        src/frontend/type_inference.h
        src/frontend/type_constraint.h
        src/frontend/ast_visitor.h
        src/frontend/union_find.h
        src/frontend/ast_pretty_printer.h
        src/frontend/stmt.h
        src/frontend/factory.h
        src/frontend/string_interner.h
        src/frontend/var.h
        src/vm/vm.cc
)
target_link_libraries(
        shiny
        argparse
)
