cmake_minimum_required(VERSION 3.26)
project(plab1 C)

set(CMAKE_C_STANDARD 11)

include_directories(startcode2023plab1)
include_directories(startcode2023plab1/lib)

add_executable(plab1
        startcode2023plab1/lib/dplist.h
        startcode2023plab1/config.h
        startcode2023plab1/datamgr.h
        startcode2023plab1/file_creator.c
        startcode2023plab1/main.c)
