cmake_minimum_required(VERSION 3.15)

project(Shiny VERSION 1.0
                  DESCRIPTION "Swift-like interpreter in C++"
                  LANGUAGES CXX)

add_executable(shiny src/main.cc)