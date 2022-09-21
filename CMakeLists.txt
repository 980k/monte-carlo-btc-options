cmake_minimum_required(VERSION 3.4)
project(montecarlo)

add_library(impliedvol STATIC src/implied_volatility.cpp)
target_include_directories(impliedvol PUBLIC "include/")
set_target_properties(impliedvol PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(pybind11)
pybind11_add_module(montecarlo src/monte_carlo.cpp)
target_link_libraries(montecarlo PRIVATE impliedvol)