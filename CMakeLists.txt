cmake_minimum_required(VERSION 3.27)
project(dataFrame)

set(CMAKE_CXX_STANDARD 17)

add_executable(dataFrame main.cpp
        dataframe.h
        dataframe.cpp
        slice.h
        arena.cpp
        arena.h
        FeatureValue.h)
