# Copyright (C) 2026
# Computer Graphics Group, University of Siegen
# For questions, contact Marina Evers <marina.evers@uni-siegen.de>
#
# Copying and distribution of this file, with or without modification, are
# permitted in any medium without royalty provided the copyright notice and this
# notice are preserved. This file is offered as-is, without any warranty.

cmake_minimum_required (VERSION 3.24)
project(CG-Praktikum LANGUAGES C CXX)

# -----------------------------------------------------------------------------
# Settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Resource directory (shaders, textures). Shaders and textures are loaded at runtime relative to this path.
set(CG_RESOURCE_PATH "${CMAKE_SOURCE_DIR}/res")

# -----------------------------------------------------------------------------
# Framework library (glbase and its dependencies)
# -----------------------------------------------------------------------------
add_subdirectory("glbase")

# -----------------------------------------------------------------------------
# Source files
#
# Every new class needs its .h AND .cpp listed here.
#   - .cpp files are compiled. If you forget one, you get a LINKER error:
#       MSVC:      "unresolved external symbol ..."
#       GCC/Clang: "undefined reference to ..."
#   - .h files are NOT compiled. They are listed only so they show up in the IDE.
#     Whether #include finds them is decided by target_include_directories below.
# -----------------------------------------------------------------------------
set(CG_SOURCES
    src/krake.cpp
    src/objects.cpp    
    src/orbit_camera.cpp
    src/primitives.cpp
    src/template_app.cpp
)

set(CG_HEADERS    
    src/animated_node.h
    src/animation.h
    src/krake.h
    src/lighting.h
    src/objects.h
    src/orbit_camera.h
    src/primitives.h
    src/template_app.h
    src/render_context.h
)

# -----------------------------------------------------------------------------
# Shaders
#
# Shaders are not compiled by CMake, they are loaded by the program at runtime.
# They are listed here only so you can open and edit them directly in the IDE.
# -----------------------------------------------------------------------------
set(CG_SHADERS
    res/shaders/phong.vert.glsl
    res/shaders/phong.frag.glsl
)

# -----------------------------------------------------------------------------
# Create an executable
# -----------------------------------------------------------------------------
add_executable(CG-Praktikum ${CG_SOURCES} ${CG_HEADERS} ${CG_SHADERS})
target_link_libraries(CG-Praktikum PRIVATE glbase)
target_include_directories(CG-Praktikum PUBLIC "src")
# Make the resource path available in C++ as the macro CG_RESOURCE_PATH
target_compile_definitions(CG-Praktikum PRIVATE CG_RESOURCE_PATH="${CG_RESOURCE_PATH}")

# Warnings for the solution code (not for the vendored libraries)
if(MSVC)
    target_compile_options(CG-Praktikum PRIVATE /W4)
else()
    target_compile_options(CG-Praktikum PRIVATE -Wall -Wextra)
endif()
