cmake_minimum_required(VERSION 3.26)
project(visualizer_1 LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakePackageConfigHelpers)

include_directories(include)
include_directories(stb)

include(cmake/ProjectOptions.cmake)
include(cmake/Libraries.cmake)
include(cmake/Embed.cmake)
include(src/CMakeLists.txt)
include(programs/CMakeLists.txt)

target_compile_definitions(math-visualizer PUBLIC STBI_NO_SIMD=1)

install(TARGETS math-visualizer math-visualizer-resources glm glm-header-only
        EXPORT mvTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/mv include/mvl
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp")

install(EXPORT mvTargets
        FILE mvTargets.cmake
        NAMESPACE mv::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mv
)

set_property(TARGET math-visualizer  PROPERTY VERSION ${version})

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/mvConfigVersion.cmake"
        VERSION "${version}"
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/mvConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mv
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/mvConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/mvConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mv
)

export(EXPORT mvTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/mvTargets.cmake"
        NAMESPACE mv::
)