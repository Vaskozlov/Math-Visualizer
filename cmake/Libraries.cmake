include(cmake/CPM.cmake)

find_package(Threads REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)

CPMFindPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 577fd3be883accf8629423ed77fcca8a24bccee2
)

CPMFindPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
)

CPMAddPackage(
        NAME unordered_dense
        GITHUB_REPOSITORY martinus/unordered_dense
        GIT_TAG 73f3cbb237e84d483afafc743f1f14ec53e12314
)

CPMAddPackage(
        NAME battery-embed
        GITHUB_REPOSITORY batterycenter/embed
        GIT_TAG 465081903d97ff1ed05e1fd5d0b3c8032a4a26a6
)

CPMAddPackage(
        NAME ccl
        GITHUB_REPOSITORY Vaskozlov/ccl-project
        GIT_TAG ba1bb9b0b7aeffc6a0992a64d01d63ebfc5fa90e
)
