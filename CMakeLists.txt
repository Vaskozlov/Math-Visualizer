cmake_minimum_required(VERSION 3.30)
project(visualizer_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

include(cmake/ProjectOptions.cmake)
include(cmake/Libs.cmake)
include(src/CMakeLists.txt)



