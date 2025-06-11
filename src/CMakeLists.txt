file(
        GLOB_RECURSE
        MV_OBJECTS
        "src/*.cpp"
)

add_library(
        math-visualizer
        STATIC
        ${MV_OBJECTS}
)

isl_set_target_flags(
        math-visualizer
        ${MV_STRICT_COMPILATION}
        ""
        ""
        ""
        ""
        ${MV_HARDENING}
        ${MV_LIBCPP}
        ${MV_UNITY_BUILD}
)

target_link_libraries(
        math-visualizer
        PUBLIC
        fmt::fmt-header-only
        glfw
        GLEW::GLEW
        glm::glm
        imgui::imgui
        astlang2
)

target_include_directories(
        math-visualizer
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
)