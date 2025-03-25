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

add_executable(
        computation-2
        programs/computation-2.cpp
)

isl_set_target_flags(
        computation-2
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
        computation-2
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        computation-2
        computation-2-resources
)

mv_embed_resources("resources" computation-2-resources)

add_executable(
        computation-2-part-2
        programs/computation-2-part-2.cpp
)

isl_set_target_flags(
        computation-2-part-2
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
        computation-2-part-2
        PUBLIC
        math-visualizer
)

b_embed_proxy_target(
        computation-2-part-2
        computation-2-part-2-resources
)

mv_embed_resources("resources" computation-2-part-2-resources)

add_executable(
        equation
        programs/equation.cpp
)

isl_set_target_flags(
        equation
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
        equation
        PUBLIC
        math-visualizer
)

add_executable(
        system
        programs/system.cpp
)

isl_set_target_flags(
        system
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
        system
        PUBLIC
        math-visualizer
)

add_executable(
        integrals
        programs/integrals.cpp
)

isl_set_target_flags(
        integrals
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
        integrals
        PUBLIC
        math-visualizer
)

