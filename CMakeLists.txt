cmake_minimum_required(VERSION 3.16)
project(voxel-engine)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/entt EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED)

# Create your game executable target as usual
add_executable(voxel-engine WIN32 
src/main.cpp
src/core/Application.cpp
)
target_include_directories(voxel-engine PRIVATE include)

# Link to the actual SDL3 library.
target_link_libraries(voxel-engine PRIVATE SDL3::SDL3 OpenGL::GL EnTT::EnTT)
