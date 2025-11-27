cmake_minimum_required(VERSION 3.28)
project(Tetris LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_library(Board board.cpp board.hpp element.hpp)
target_link_libraries(Board SFML::Graphics)

add_library(Element element.cpp element.hpp)
target_link_libraries(Element SFML::Graphics)

add_executable(Main main.cpp)
target_compile_features(Main PRIVATE cxx_std_17)
target_link_libraries(Main PRIVATE Board Element SFML::Graphics)
