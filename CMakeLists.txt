cmake_minimum_required(VERSION 3.30)
project(Shiny VERSION 1.0
              DESCRIPTION "Swift-like interpreter in C++"
              LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(tests)
