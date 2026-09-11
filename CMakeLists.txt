cmake_minimum_required(VERSION 3.20)

project(NBodySimulation LANGUAGES CXX)

# Global C++ stamdard settings

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directory

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# SFML dependency resolution

if(DEFINED ENV{SFML_ROOT} AND NOT DEFINED SFML_ROOT)
    set(SFML_ROOT "$ENV{SFML_ROOT}")
endif()

if(SFML_ROOT)
    set(SFML_DIR "${SFML_ROOT}/lib/cmake/SFML")
endif()

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG QUIET)

if(NOT SFML_FOUND)
    message(STATUS "SFML not found locally — fetching SFML 3.1.0 from source via FetchContent")

    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SFML)
else()
    message(STATUS "Using local SFML installation found via SFML_ROOT: ${SFML_ROOT}")
endif()

# Simulation / Test executable

add_executable(simulation_test
    tests/simulation_testing.cpp
    src/simulation.cpp
    src/particle.cpp
)

target_include_directories(simulation_test PRIVATE include)
target_compile_features(simulation_test PRIVATE cxx_std_17)

# SFML experiment executable

add_executable(sfml_create_window
    experiments/sfml_create_window.cpp
)

target_compile_features(sfml_create_window PRIVATE cxx_std_17)

target_link_libraries(sfml_create_window PRIVATE SFML::Graphics)

add_custom_command(TARGET sfml_create_window POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::Graphics>
        $<TARGET_FILE_DIR:sfml_create_window>

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::Window>
        $<TARGET_FILE_DIR:sfml_create_window>

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::System>
        $<TARGET_FILE_DIR:sfml_create_window>
)