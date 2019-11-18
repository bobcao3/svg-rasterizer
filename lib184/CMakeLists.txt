cmake_minimum_required(VERSION 3.10)
project(lib184)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options( -DGLM_FORCE_SSE4 -O3 )

# Source code
set(IMGUISRC imgui/examples/imgui_impl_opengl3.cpp
             imgui/examples/imgui_impl_glfw.cpp
             imgui/imgui.cpp
             imgui/imgui_draw.cpp
             imgui/imgui_widgets.cpp)
set(SRCFILES application.cpp
             color.cpp
             tinyxml2/tinyxml2.cpp
             lodepng.cpp
             base64.cpp)

add_library(lib184 ${SRCFILES} ${IMGUISRC})

target_include_directories(lib184 PUBLIC .)
target_include_directories(lib184 PUBLIC imgui/)
target_include_directories(lib184 PUBLIC glm/)
target_include_directories(lib184 PUBLIC tinyxml2/)

# Libraries
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_link_libraries(lib184 PUBLIC glfw)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_include_directories(lib184 PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(lib184 PUBLIC ${OPENGL_gl_LIBRARY})

add_subdirectory(glad)
target_link_libraries(lib184 PUBLIC glad)

add_subdirectory(glm)
target_include_directories(lib184 PUBLIC glm)
