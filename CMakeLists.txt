cmake_minimum_required(VERSION 3.26)
project(visualizer_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

include(cmake/ProjectOptions.cmake)
include(cmake/Libraries.cmake)
include(src/CMakeLists.txt)
include(programs/CMakeLists.txt)

