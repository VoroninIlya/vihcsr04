cmake_minimum_required(VERSION 3.22)

project(tst_vihcsr04)

#include(CodeCoverage)
include(CTest)

if(PROJECT_IS_TOP_LEVEL)
# Set as executable
add_subdirectory(${UNITY_ROOT_PATH} ${${EXECUTABLE}_BINARY_DIR}/modules/unity)
endif()

# Add core subdir
add_subdirectory(core)

add_executable(tst_vihcsr04)
enable_testing()

# Add key executable block
target_sources(tst_vihcsr04 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/tests/main/main.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/tst_vihcsr04.c
)

# Add key include paths
target_include_directories(tst_vihcsr04 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/core/str/inc
    ${UNITY_ROOT_PATH}/src
    ${UNITY_ROOT_PATH}/extras/fixture/src
    ${UNITY_ROOT_PATH}/extras/memory/src
)

# Compilation definition information
target_compile_definitions(tst_vihcsr04 PUBLIC
    WIN32
    _DEBUG
    CONSOLE
)

# Compiler options
target_compile_options(tst_vihcsr04 PRIVATE
    -g
    --coverage
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(
  tst_vihcsr04 vihcsr04 unity -g -coverage -lgcov)

add_test(NAME VIHCSR04_Init COMMAND tst_vibuttonctrl "--gtest_filter=VIHCSR04_Init.*")
