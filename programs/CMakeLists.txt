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
        spline
        programs/spline.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
    target_compile_options(spline PRIVATE -march=native)
endif ()

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

add_executable(
        spline3d
        programs/spline3d.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
    target_compile_options(spline3d PRIVATE -march=native)
endif ()

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

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
    target_compile_options(texture3d PRIVATE -march=native)
endif ()

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
