file(
        GLOB_RECURSE
        MV_OBJECTS
        "src/*.cpp"
)

add_executable(
        math-visualizer
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
)

b_embed_proxy_target(
        math-visualizer
        math-visualizer-resources
)

b_embed(
        math-visualizer-resources
        resources/shaders/vertex.vert
)

b_embed(
        math-visualizer-resources
        resources/shaders/fragment.frag
)

b_embed(
        math-visualizer-resources
        resources/shaders/colored_shader.vert
)
