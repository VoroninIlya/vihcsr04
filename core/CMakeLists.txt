cmake_minimum_required(VERSION 3.22)

project(vihcsr04)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(vihcsr04 INTERFACE)
target_sources(vihcsr04 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/vihcsr04.c)
target_include_directories(vihcsr04 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/inc)

project(vihcsr04cpp)

add_library(vihcsr04cpp INTERFACE)
target_sources(vihcsr04cpp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/vihcsr04.cpp)
target_include_directories(vihcsr04cpp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/inc)

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")