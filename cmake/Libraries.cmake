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

CPMAddPackage(
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
        NAME ccl
        GITHUB_REPOSITORY Vaskozlov/ccl-project
        GIT_TAG 044fae8c996e5803e7c6a4f2ae7d3ee22d3525d1
        OPTIONS "CCL_STATIC_LIBRARY ON" "ISL_STATIC_LIBRARY ON"
)
