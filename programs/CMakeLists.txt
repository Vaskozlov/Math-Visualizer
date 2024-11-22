add_executable(
        mandelbrot_fractal
        programs/mandelbrot_fractal.cpp
)

isl_set_target_flags(
        mandelbrot_fractal
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
        mandelbrot_fractal
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        mandelbrot_fractal
        mandelbrot_fractal-resources
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/default.vert
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/with_offset.vert
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/julia.frag
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/mandelbrot.frag
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/newton.frag
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/common.glsl
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/newton_classic.frag
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/fractal/newton_sin.frag
)

b_embed(
        mandelbrot_fractal-resources
        resources/fonts/JetBrainsMono-Medium.ttf
)

add_executable(
        function_gradient
        programs/function_gradient.cpp
)

isl_set_target_flags(
        function_gradient
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
        function_gradient
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        function_gradient
        function_gradient-resources
)

b_embed(
        function_gradient-resources
        resources/shaders/vertex.vert
)

b_embed(
        function_gradient-resources
        resources/shaders/fragment.frag
)

b_embed(
        function_gradient-resources
        resources/shaders/colored_shader.vert
)


b_embed(
        function_gradient-resources
        resources/shaders/static_instance.vert
)

b_embed(
        function_gradient-resources
        resources/fonts/JetBrainsMono-Medium.ttf
)

add_executable(
        physics
        programs/physics.cpp
)

isl_set_target_flags(
        physics
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
        physics
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        physics
        physics-resources
)

b_embed(
        physics-resources
        resources/shaders/vertex.vert
)

b_embed(
        physics-resources
        resources/shaders/fragment.frag
)

b_embed(
        physics-resources
        resources/shaders/colored_shader.vert
)


b_embed(
        physics-resources
        resources/shaders/static_instance.vert
)

b_embed(
        physics-resources
        resources/fonts/JetBrainsMono-Medium.ttf
)
