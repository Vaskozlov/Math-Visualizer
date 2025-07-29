
add_executable(
        spline
        programs/spline.cpp
)

isl_set_target_flags(
        spline
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
        spline
        PUBLIC
        math-visualizer
)

if(EMSCRIPTEN)
        set_target_properties(
                spline
                PROPERTIES
                SUFFIX ".html"
        )

        target_link_options(
                spline PRIVATE
                --preload-file ${CMAKE_SOURCE_DIR}/resources@/resources
        )

         target_link_options(
                spline
                PRIVATE
                --shell-file ${CMAKE_SOURCE_DIR}/emscripten/shell_minimal.html
        )
else()
        if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
                target_compile_options(spline PRIVATE -march=native)
        endif()
endif()

add_executable(
        spline3d
        programs/spline3d.cpp
)

isl_set_target_flags(
        spline3d
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
        spline3d
        PUBLIC
        math-visualizer
)

add_executable(
        texture3d
        programs/texture3d.cpp
)

isl_set_target_flags(
        texture3d
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
        texture3d
        PUBLIC
        math-visualizer
)

add_executable(
        waterfall
        programs/waterfall.cpp
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

if(EMSCRIPTEN)
        set_target_properties(
                waterfall
                PROPERTIES
                SUFFIX ".html"
        )

        target_link_options(
                waterfall PRIVATE
                --preload-file ${CMAKE_SOURCE_DIR}/resources@/resources
        )

         target_link_options(
                waterfall
                PRIVATE
                --shell-file ${CMAKE_SOURCE_DIR}/emscripten/shell_minimal.html
        )
else()
        if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
                target_compile_options(waterfall PRIVATE -march=native)
        endif()
endif()