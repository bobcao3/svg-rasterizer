cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Rasterizer VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source code
set(SRCFILES src/main.cpp
             src/RasterSVG.cpp
             src/renderers/line.cpp)

add_executable(Rasterizer ${SRCFILES})

target_include_directories(Rasterizer PUBLIC lib)
target_include_directories(Rasterizer PUBLIC lib/imgui/)

# Libraries
add_subdirectory(lib184)
target_link_libraries(Rasterizer PUBLIC lib184)

target_include_directories(Rasterizer PUBLIC nanosvg/src)