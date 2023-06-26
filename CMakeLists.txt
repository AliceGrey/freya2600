cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(Freya2600)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 2.0.6 CONFIG REQUIRED)

file(GLOB_RECURSE
    _source_list
    "Source/*.hpp"
    "Source/*.cpp"
)

add_executable(
    Freya2600
    ${_source_list}
)

target_include_directories(
    Freya2600
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Source
)

target_link_libraries(
    Freya2600
    PRIVATE
        SDL2::SDL2
)