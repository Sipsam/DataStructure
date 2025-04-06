cmake_minimum_required(VERSION 3.30)
project(study C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

add_executable(study
        main.c
        originVer.c
        originVer.h
        improvedVer.c
        improvedVer.h
        linkedVer.c
        linkedVer.h
)
