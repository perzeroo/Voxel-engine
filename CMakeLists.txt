cmake_minimum_required(VERSION 3.16)
project(voxel-engine)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Clang detected, enabling AddressSanitizer")

    # Enable AddressSanitizer
    # add_compile_options(-gcodeview -gdwarf-2 -fsanitize=address -fno-omit-frame-pointer)
    # add_link_options(-fsanitize=address)
endif()

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



file(GLOB SHADERS "{PROJECT_SOURCE_DIR}/assets/shaders/*")
foreach(shader ${SHADERS})
  add_custom_command(TARGET voxel-engine POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${shader} $<TARGET_FILE_DIR:voxel-engine>/assets/shaders)
endforeach()

if(MINGW)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_compile_options(-m64)
    else()
        add_compile_options(-m32)
    endif()
endif()

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863)

FetchContent_MakeAvailable(glm)

# option(TRACY_ENABLE ON)
set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
# option(TRACY_ON_DEMAND ON)

FetchContent_Declare(
  tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG c556831ddc6fe26d2fce01c14c97205a9dad46d5
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(tracy)

# This assumes the SDL source is available in vendored/SDL
set(SDL_OPENGLES OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/entt EXCLUDE_FROM_ALL)
add_subdirectory(vendored/glad)


find_package(OpenGL REQUIRED)

# Create your game executable target as usual
add_executable(voxel-engine WIN32 
src/main.cpp
src/core/Application.cpp
src/core/Window.cpp
src/engine/Camera.cpp
src/engine/chunk/ChunkMeshBuilder.cpp
src/engine/chunk/ChunkMesh.cpp
src/engine/Render.cpp
src/engine/File.cpp
src/engine/chunk/ChunkData.cpp
src/engine/chunk/ChunkManager.cpp
src/engine/world/WorldGenerator.cpp
src/engine/ShaderManager.cpp
src/engine/ThreadPool.cpp
vendored/imgui/src/imgui_impl_opengl3.cpp
vendored/imgui/src/imgui_impl_sdl3.cpp
vendored/imgui/src/imgui.cpp
vendored/imgui/src/imgui_draw.cpp
vendored/imgui/src/imgui_tables.cpp
vendored/imgui/src/imgui_widgets.cpp
vendored/imgui/src/imgui_demo.cpp
)
target_include_directories(voxel-engine PRIVATE include vendored/imgui/include)

# target_compile_options(voxel-engine PRIVATE -Wall -Wextra -Wno-unused-parameter -fno-elide-type -ftemplate-backtrace-limit=0>)
# Link to the actual SDL3 library.
target_link_libraries(voxel-engine PRIVATE SDL3::SDL3 EnTT::EnTT glad OpenGL::GL glm::glm Tracy::TracyClient)

target_compile_options(SDL3-static PRIVATE -Wno-unused-parameter -Wno-deprecated-declarations)
target_compile_definitions(voxel-engine PRIVATE TRACY_ENABLE)
