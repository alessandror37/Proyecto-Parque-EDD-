cmake_minimum_required(VERSION 4.1)
project(Proyecto_EDD_IBClandia C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -pedantic-errors)
endif()


add_executable(Proyecto_EDD_IBClandia main.c)
