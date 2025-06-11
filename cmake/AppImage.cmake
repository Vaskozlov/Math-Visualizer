set(LINUXDEPLOY "${CMAKE_SOURCE_DIR}/tools/linuxdeploy-x86_64.AppImage")

find_program(LINUXDEPLOY linuxdeploy linuxdeploy-x86_64.AppImage)

set(CPACK_GENERATOR "AppImage")

if(LINUXDEPLOY)

    install(FILES data/spline.desktop DESTINATION share/applications)
    install(FILES data/spline.png DESTINATION share/icons/hicolor/512x512/apps)
    install(DIRECTORY resources/ DESTINATION share/resources)

    # message(WARNING "")

    # Create AppImage after build
    add_custom_target(
        appimage
        COMMAND mkdir -p AppDir
        COMMAND DESTDIR=${CMAKE_BINARY_DIR}/AppDir ninja install
        COMMAND ${LINUXDEPLOY}
        --appdir=${CMAKE_BINARY_DIR}/AppDir
        --executable=${CMAKE_BINARY_DIR}/AppDir/usr/local/bin/spline
        --desktop-file=${CMAKE_SOURCE_DIR}/data/spline.desktop
        --icon-file=${CMAKE_SOURCE_DIR}/data/spline.png
        --output=appimage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating AppImage..."
    )
endif()

