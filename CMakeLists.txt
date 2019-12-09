cmake_minimum_required(VERSION 3.10)

project(geo)

find_package(GLEW REQUIRED)

if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

set (CMAKE_CXX_STANDARD 11)

include_directories(include)

file(GLOB SOURCES "src/*.cc")

set(CMAKE_CXX_FLAGS " -Wextra -g -O3")

add_executable(geo.exe ${SOURCES})

target_link_libraries(geo.exe -lGL -lsfml-graphics -lsfml-window -lsfml-system)
