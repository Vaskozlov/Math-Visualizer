include(cmake/CPM.cmake)

find_package(Threads REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)

CPMAddPackage(
        NAME ccl
        GITHUB_REPOSITORY Vaskozlov/ccl-project
        GIT_TAG main
)

CPMAddPackage(
        NAME unordered_dense
        GITHUB_REPOSITORY martinus/unordered_dense
        GIT_TAG main
)

CPMAddPackage(
        NAME battery-embed
        GITHUB_REPOSITORY batterycenter/embed
        GIT_TAG main
)

include(${ccl_SOURCE_DIR}/cmake/Libraries.cmake)
