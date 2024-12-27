cmake_minimum_required(VERSION 3.28)

project(Tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.5.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)


add_library(Board board.cpp board.hpp element.hpp)
target_link_libraries(Board SFML::Graphics)

add_library(Element element.cpp element.hpp)
target_link_libraries(Element SFML::Graphics)

add_executable(Main main.cpp)

target_link_libraries(Main PRIVATE Board Element SFML::Graphics)
