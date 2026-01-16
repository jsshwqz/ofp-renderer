cmake_minimum_required(VERSION 3.16)
project(OFPRenderer VERSION 1.0.0 LANGUAGES CXX ASM)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Set module path for custom find modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find required packages
find_package(Vulkan REQUIRED)
find_package(DirectX9 REQUIRED)

# Collect source files
file(GLOB SOURCES
    "src/*.cpp"
    "src/*.c"
    "include/*.h"
)

# Create shared library
add_library(ofp_renderer SHARED ${SOURCES})

# Include directories
target_include_directories(ofp_renderer PRIVATE
    "include"
    ${Vulkan_INCLUDE_DIRS}
    ${DirectX9_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(ofp_renderer PRIVATE
    Vulkan::Vulkan
    ${DirectX9_LIBRARIES}
)

# Compile definitions
target_compile_definitions(ofp_renderer PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    VK_USE_PLATFORM_WIN32_KHR
)

# MSVC-specific options
if(MSVC)
    target_compile_options(ofp_renderer PRIVATE
        /W4           # Warning level 4
        /WX-          # Warnings not as errors (change to /WX for strict)
        /EHsc         # Exception handling
        /MP           # Multi-processor compilation
        /O2           # Optimization level 2
        /Oi           # Intrinsic functions
        /GL           # Whole program optimization
        /Gy           # Function-level linking
    )
    
    # Linker options
    set_target_properties(ofp_renderer PROPERTIES
        LINK_FLAGS "/OPT:REF /OPT:ICF"
    )
endif()

# Install target
install(TARGETS ofp_renderer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Create configuration file template
configure_file(
    "config/ofp_renderer.ini.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ofp_renderer.ini"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ofp_renderer.ini"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/ofp-renderer"
)

# Print configuration summary
message(STATUS "")
message(STATUS "OFP Renderer Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Vulkan SDK: ${Vulkan_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
