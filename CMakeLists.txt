cmake_minimum_required(VERSION 3.10)
project(intro-oop1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Week 1
add_executable(week1 week1/25310023_25310057_25310043.cpp)
add_executable(week2 week2/25310023_25310057_25310043.cpp)
add_executable(week3 week3/source/main.cpp
        week3/header/Equation.h
        week3/header/EquationIO.h
        week3/source/EquationIO.cpp
        week3/source/Equation.cpp
)