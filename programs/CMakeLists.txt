function(mv_embed_resources PATH TARGET_NAME)
    file(GLOB_RECURSE RESOURCE_FILES "${PATH}/*")

    foreach (RESOURCE_FILE ${RESOURCE_FILES})
        get_filename_component(FILENAME "${RESOURCE_FILE}" NAME)
        if (NOT FILENAME MATCHES "^\\..*")
            file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}" "${RESOURCE_FILE}")
            b_embed(
                    ${TARGET_NAME}
                    ${REL_PATH}
            )
        endif ()
    endforeach ()
endfunction()

file(GLOB RESOURCE_FILES "resources/*")

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

mv_embed_resources("resources" mandelbrot_fractal-resources)

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

mv_embed_resources("resources" function_gradient-resources)

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

mv_embed_resources("resources"  physics-resources)

add_executable(
        dft
        programs/dft.cpp
)

isl_set_target_flags(
        dft
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
        dft
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        dft
        dft-resources
)

mv_embed_resources("resources"  dft-resources)
