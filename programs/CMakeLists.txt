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
        PRIVATE
        fmt::fmt
        glfw
        GLEW::GLEW
        glm::glm
        imgui::imgui
        math-visualizer
)

b_embed_proxy_target(
        mandelbrot_fractal
        mandelbrot_fractal-resources
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/mandelbrot_fractal.vert
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/mandelbrot_fractal.frag
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/julia_fractal.vert
)

b_embed(
        mandelbrot_fractal-resources
        resources/shaders/julia_fractal.frag
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
        PRIVATE
        fmt::fmt
        glfw
        GLEW::GLEW
        glm::glm
        imgui::imgui
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
