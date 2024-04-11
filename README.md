# Real-Time Responsive Grass Renderer
### Made By Luis Tadeo Sanchez

## About
This project is an implementation of the paper, [Responsive real-time grass rendering for general 3D scenes](https://dl.acm.org/doi/10.1145/3023368.3023380) by Jahrmann & Wimmer.

## Build
To build this project, you will need CMake for setting up the build and the [VulkanSDK](https://vulkan.lunarg.com/). Clone the project and all of its subdirectories:
```
git clone --recurse-submodules 
```
Create a build directory and from that build directory call cmake
```
cmake build ..
```
### Windows
Open the visual studio project that was generated and build the project in Debug

### MacOS
From that build directory, call make to build the project
