file(
        GLOB_RECURSE
        MV_OBJECTS
        "src/mv/*.cpp"
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
        glm::glm
        imgui
        ccl
        Threads::Threads
)

if(EMSCRIPTEN)
        target_link_options(math-visualizer INTERFACE -sUSE_GLFW=3)
        target_link_options(math-visualizer INTERFACE -sASSERTIONS=1)

        target_compile_options(
                math-visualizer
                INTERFACE
                -matomics
        )
else()
        target_link_libraries(
                math-visualizer
                PUBLIC
                glfw
                libglew_static
        )

endif()

target_include_directories(
        math-visualizer
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
)
