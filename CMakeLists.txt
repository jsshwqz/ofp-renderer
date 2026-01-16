cmake_minimum_required(VERSION 3.16)
project(OFPRenderer VERSION 1.0.0 LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(SOURCES
    src/dllmain.cpp
    src/post_processing.cpp
    src/vulkan_renderer.cpp
)

add_library(ofp_renderer SHARED ${SOURCES})

target_include_directories(ofp_renderer PRIVATE
    "include"
)

target_compile_definitions(ofp_renderer PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    VK_USE_PLATFORM_WIN32_KHR
    VK_NO_PROTOTYPES
)

if(MSVC)
    target_compile_options(ofp_renderer PRIVATE
        /W4
        /WX-
        /EHsc
        /MP
        /O2
        /Oi
        /GL
        /Gy
    )
    
    set_target_properties(ofp_renderer PROPERTIES
        LINK_FLAGS "/OPT:REF /OPT:ICF"
    )
endif()

install(TARGETS ofp_renderer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

configure_file(
    "config/ofp_renderer.ini.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ofp_renderer.ini"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ofp_renderer.ini"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/ofp-renderer"
)

message(STATUS "")
message(STATUS "OFP Renderer Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Using bundled Vulkan header")
message(STATUS "")
