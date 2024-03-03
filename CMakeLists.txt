cmake_minimum_required(VERSION 3.26)
project(LabsDistribuidos C)

set(CMAKE_C_STANDARD 11)

include_directories("Lab 1 - C")

add_executable(LabsDistribuidos
        "ejercicio eveluable1/clave.h"
        "Lab 1 - C/p2.c"
        "Lab 1 - C/p3.c"
        "Lab 1 - C/p4.c")
