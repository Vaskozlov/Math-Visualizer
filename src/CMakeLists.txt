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
        PRIVATE
        fmt::fmt
        glfw
        GLEW::GLEW
        glm::glm
        imgui::imgui
        ccl
        astlang2
        unordered_dense::unordered_dense
)

b_embed_proxy_target(
        math-visualizer
        math-visualizer-resources
)

b_embed(
        math-visualizer-resources
        resources/grammar/mvl.cclp
)
