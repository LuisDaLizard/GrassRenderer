# Real-Time Responsive Grass Renderer
### Made By Luis Tadeo Sanchez

## About
This project is an implementation of the paper, Responsive real-time grass rendering for general 3D scenes by Jahrmann & Wimmer.

## Build
To build this project, you will need CMake for setting up the build. Clone the project and all of its subdirectories:
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

## Issues
In order to progress any further on this project and keep it cross-platform for Mac, Windows, and Linux, the project needs to switch from OpenGL to Vulkan. There is a Vulkan branch currently in progress that uses my Vulkan library that's still in development.
