# FindVulkan.cmake
# Finds the Vulkan SDK

if(Vulkan_INCLUDE_DIR)
    set(Vulkan_FOUND TRUE)
else()
    find_path(Vulkan_INCLUDE_DIR
        NAMES vulkan/vulkan.h
        PATHS
            $ENV{VULKAN_SDK}/Include
            /usr/include
            /usr/local/include
            /opt/VulkanSDK/Include
        DOC "Vulkan include directory"
    )

    find_library(Vulkan_LIBRARY
        NAMES vulkan
        PATHS
            $ENV{VULKAN_SDK}/Lib
            /usr/lib
            /usr/local/lib
            /opt/VulkanSDK/Lib
        DOC "Vulkan library"
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Vulkan
        REQUIRED_VARS
            Vulkan_INCLUDE_DIR
            Vulkan_LIBRARY
    )

    mark_as_advanced(Vulkan_INCLUDE_DIR Vulkan_LIBRARY)
endif()

if(Vulkan_FOUND)
    set(Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIR})
    set(Vulkan_LIBRARIES ${Vulkan_LIBRARY})
endif()
