cmake_minimum_required(VERSION 3.13)
project(Client C)

set(CMAKE_C_STANDARD 99)

add_executable(Client main.c Utils.c Utils.h)