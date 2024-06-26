cmake_minimum_required(VERSION 3.23)
project(GrassRenderer)

add_subdirectory(libs/ShlibVK)

find_package(Vulkan)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        src/Application.cpp
        src/Model.cpp
        src/Grass.cpp
        src/Cluster.cpp
        src/Wind.cpp

        libs/imgui/imgui.cpp
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_vulkan.cpp
        )

set(INCLUDES
        include/
        libs/ShlibVK/include/
        libs/imgui/
        libs/imgui/backends/
        libs/OBJ-Loader/
        ${Vulkan_INCLUDE_DIRS}
        )

set(LIBS
        ShlibVK
        ${Vulkan_LIBRARIES}
        )

set(RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
        set(RESOURCE_DIR ${RESOURCE_DIR}/Debug)
endif()

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${LIBS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION ${RESOURCE_DIR})