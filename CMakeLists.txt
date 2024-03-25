cmake_minimum_required(VERSION 3.23)
project(GrassRenderer)

add_subdirectory(libs/ShlibGraphics)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        libs/imgui/imgui.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_opengl3.cpp
        )

set(INCLUDES
        include/
        libs/ShlibGraphics/include/
        libs/imgui/
        libs/imgui/backends/
        )

set(LIBS
        ShlibGraphics
        )

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
