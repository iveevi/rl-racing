# Set CMake version
cmake_minimum_required(VERSION 3.14)

# Set the right flags
set(CMAKE_CXX_FLAGS "-pthread -std=c++17")

# Allow dynamic linking
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

# Set compiler
set(CMAKE_CXX_COMPILER "g++-8")

# Agent library
project(racing)
add_library(racing SHARED
	source/agent.cpp
	source/master.cpp
	source/lib.cpp
)

# Godot bindings
find_library(GODOT_LIBRARY godot-cpp.linux.debug.64 REQUIRED)
target_link_libraries(racing PUBLIC ${GODOT_LIBRARY})

# Source include
include_directories(PUBLIC source)

# Zhetapi include
include_directories(PUBLIC zhetapi/engine)

# Godot include
include_directories(PUBLIC godot-cpp/godot_headers)
include_directories(PUBLIC godot-cpp/include)
include_directories(PUBLIC godot-cpp/include/gen)
include_directories(PUBLIC godot-cpp/include/core)
