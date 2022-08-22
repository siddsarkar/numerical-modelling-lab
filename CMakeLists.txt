cmake_minimum_required(VERSION 3.23)
project(numerical_modelling_lab)

set(CMAKE_CXX_STANDARD 23)

add_executable(numerical_modelling_lab main.cpp pbPlot/pbPlots.cpp pbPlot/supportLib.cpp)
