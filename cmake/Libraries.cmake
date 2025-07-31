include(cmake/CPM.cmake)

find_package(Threads REQUIRED)

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 11.2.0
)

if(NOT EMSCRIPTEN)
        find_package(OpenGL REQUIRED)

        CPMAddPackage(
                NAME glfw
                GITHUB_REPOSITORY glfw/glfw
                GIT_TAG 3.4
                OPTIONS "BUILD_SHARED_LIBS OFF" "GLFW_LIBRARY_TYPE STATIC"
        )

        CPMAddPackage(
                NAME glew
                GITHUB_REPOSITORY Perlmint/glew-cmake
                GIT_TAG glew-cmake-2.2.0
                OPTIONS "-Dglew-cmake_BUILD_STATIC=ON"
        )
endif()

CPMAddPackage(
        NAME imgui
        GITHUB_REPOSITORY ocornut/imgui
        GIT_TAG v1.91.9-docking
)

add_library(
        imgui
        STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(
        imgui
        PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${imgui_SOURCE_DIR}/misc/cpp
        ${glfw_SOURCE_DIR}/include
)

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 1.0.1
)

CPMAddPackage(
        NAME unordered_dense
        GITHUB_REPOSITORY martinus/unordered_dense
        GIT_TAG v4.5.0
)

CPMAddPackage(
        NAME ccl
        GITHUB_REPOSITORY Vaskozlov/ccl-project
        GIT_TAG 6842f54a6a13ad9f0cc98e32a81d67dd293192b7
        OPTIONS "CCL_STATIC_LIBRARY ON" "ISL_STATIC_LIBRARY ON"
)
