cmake_minimum_required(VERSION 3.26)
project(ex1 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ex1
        dplist.c
        dplist.h
        program.c)
