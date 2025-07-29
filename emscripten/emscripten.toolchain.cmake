# emscripten.toolchain.cmake
cmake_minimum_required(VERSION 3.15)

set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler settings
set(CMAKE_C_COMPILER "$ENV{EMSDK}/upstream/emscripten/emcc")
set(CMAKE_CXX_COMPILER "$ENV{EMSDK}/upstream/emscripten/em++")
set(CMAKE_AR "$ENV{EMSDK}/upstream/emscripten/emar")
set(CMAKE_RANLIB "$ENV{EMSDK}/upstream/emscripten/emranlib")
set(CMAKE_CXX_COMPILER_AR ${CMAKE_AR})
set(CMAKE_CXX_COMPILER_RANLIB ${CMAKE_RANLIB})

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s WASM=1 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1 -pthread -sPTHREAD_POOL_SIZE=8")

# Find packages in Emscripten root
set(CMAKE_FIND_ROOT_PATH "$ENV{EMSDK}/upstream/emscripten/cache/sysroot")

# Adjust behavior for cross-compiling
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Emscripten-specific CMake variables
set(EMSCRIPTEN 1)