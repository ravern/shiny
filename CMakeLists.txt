cmake_minimum_required(VERSION 3.15)

project(Shiny VERSION 1.0
              DESCRIPTION "Swift-like interpreter in C++"
              LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################
# DEPENDENCIES #
################

include(FetchContent)

FetchContent_Declare(argparse GIT_REPOSITORY https://github.com/p-ranav/argparse.git)
FetchContent_MakeAvailable(argparse)

##############
# EXECUTABLE #
##############

include_directories(include
                    ${argparse_SOURCE_DIR}/include)
add_executable(shiny src/main.cc)
target_link_libraries(shiny argparse)