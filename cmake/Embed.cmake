function(mv_embed_resources PATH TARGET_NAME)
    file(GLOB_RECURSE RESOURCE_FILES "${PATH}/*")

    foreach (RESOURCE_FILE ${RESOURCE_FILES})
        get_filename_component(FILENAME "${RESOURCE_FILE}" NAME)
        if (NOT FILENAME MATCHES "^\\..*")
            file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${RESOURCE_FILE}")
            b_embed(
                    ${TARGET_NAME}
                    ${REL_PATH}
            )
        endif ()
    endforeach ()
endfunction()

file(GLOB RESOURCE_FILES "resources/*")