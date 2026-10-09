# Copyright (C) 2026
# Computer Graphics Group, University of Siegen
# For questions, contact Marina Evers <marina.evers@uni-siegen.de>
#
# Copying and distribution of this file, with or without modification, are
# permitted in any medium without royalty provided the copyright notice and this
# notice are preserved. This file is offered as-is, without any warranty.

cmake_minimum_required (VERSION 3.24)
set(PROJECT_NAME "glbase")
project(${PROJECT_NAME} CXX)

# -----------------------------------------------------------------------------
# Settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # build as static library


# -----------------------------------------------------------------------------
# Add dependencies
# -----------------------------------------------------------------------------
# OPENGL_INCLUDE_DIR
find_package(OpenGL REQUIRED)

# GLFW: window, OpenGL context and input handling
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
# On Linux, build the X11 backend only by default (also works under XWayland).
# Enable GLFW_BUILD_WAYLAND manually if wayland-scanner and the Wayland dev packages are installed.
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Build GLFW with native Wayland support")
add_subdirectory("vendor/glfw")

# Glew
add_subdirectory("vendor/glew/build/cmake")

# GLM
add_subdirectory("vendor/glm")

# ImGui
add_subdirectory("vendor/imgui")
target_include_directories(imgui PUBLIC "vendor/glm")

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
add_library(${PROJECT_NAME} STATIC
	"src/application.cpp"
	"src/geometries.cpp"
	"src/geomload.cpp"
	"src/gltool.cpp"
	"src/mesh.cpp"
	"src/shader.cpp" 
	
	"src/texture.cpp"

	"vendor/tinyobjloader/tiny_obj_loader.cpp"
	"vendor/ply/plyfile.cpp"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC 
	NOMINMAX
	GLEW_STATIC
	GLEW_NO_GLU
	GLFW_INCLUDE_NONE
	GLM_ENABLE_EXPERIMENTAL
)

target_include_directories(${PROJECT_NAME} PRIVATE "src")
target_include_directories(${PROJECT_NAME} PUBLIC
	"include"
	"vendor"
	"vendor/tinyobjloader"
	"vendor/ply"
    "vendor/glew/include"
	"vendor/glm"
	"vendor/imgui"
	${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	glew
	imgui
	glfw
    ${OPENGL_gl_LIBRARY}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE CG_RESOURCE_PATH="${CG_RESOURCE_PATH}")
