include(cmake/CPM.cmake)

find_package(Threads REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

CPMAddPackage(
        NAME isl
        GITHUB_REPOSITORY Vaskozlov/ISL
        GIT_TAG main
)

CPMAddPackage(
        NAME battery-embed
        GITHUB_REPOSITORY batterycenter/embed
        GIT_TAG main
)

include(${isl_SOURCE_DIR}/cmake/CompilerOptions.cmake)
include(${isl_SOURCE_DIR}/cmake/InterproceduralOptimization.cmake)
include(${isl_SOURCE_DIR}/cmake/ProjectOptions.cmake)

include_directories(${isl_SOURCE_DIR}/include)
