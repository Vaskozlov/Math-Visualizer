option(MV_LIBCPP "Use libc++ when compiling with clang" OFF)
option(MV_STRICT_COMPILATION "Enable -Werror strict compilation" OFF)
option(MV_PRECOMPILED_HEADERS "Use precompiled headers" ON)
option(MV_ADDRESS_SANITIZER "Use sanitizers" OFF)
option(MV_THREAD_SANITIZER "Use thread sanitizers" OFF)
option(MV_ENABLE_IPO "Enable IPO/LTO" OFF)
option(MV_HARDENING "Enable hardening" OFF)
option(MV_STATIC_LIBRARY "Create static version of library " ${MSVC})
option(MV_UNITY_BUILD "Use unity build for targets " ON)
option(MV_INCLUDE_BENCHMARK "Include benchmark target" OFF)

message(STATUS "CC " ${CMAKE_C_COMPILER})
message(STATUS "CXX " ${CMAKE_CXX_COMPILER})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Strict compilation? " ${MV_STRICT_COMPILATION})
message(STATUS "Precompiled headers? " ${MV_PRECOMPILED_HEADERS})
message(STATUS "Static version? " ${MV_STATIC_LIBRARY})
message(STATUS "Enable hardening?" ${MV_HARDENING})
message(STATUS "Address sanitizer? " ${MV_ADDRESS_SANITIZER})
message(STATUS "Thread sanitizer? " ${MV_THREAD_SANITIZER})
message(STATUS "Unity build? " ${MV_UNITY_BUILD})
message(STATUS "Include benchmark? " ${MV_INCLUDE_BENCHMARK})

