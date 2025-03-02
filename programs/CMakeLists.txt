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
        function_visualizer
        programs/function_visualizer.cpp
)

isl_set_target_flags(
        function_visualizer
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
        function_visualizer
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        function_visualizer
        function_visualizer-resources
)

mv_embed_resources("resources" function_visualizer-resources)

add_executable(
        waterfall
        programs/waterfall_application.cpp
)

isl_set_target_flags(
        waterfall
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
        waterfall
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        waterfall
        waterfall-resources
)

mv_embed_resources("resources"  waterfall-resources)

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
