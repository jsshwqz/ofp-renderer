# FindDirectX9.cmake
# Finds DirectX 9 SDK

if(DirectX9_INCLUDE_DIR)
    set(DirectX9_FOUND TRUE)
else()
    find_path(DirectX9_INCLUDE_DIR
        NAMES d3d9.h d3dx9.h
        PATHS
            $ENV{DXSDK_DIR}/Include
            "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
            "C:/Program Files/Microsoft DirectX SDK (June 2010)/Include"
        DOC "DirectX 9 include directory"
    )

    find_library(DirectX9_LIBRARY
        NAMES d3d9 d3dx9
        PATHS
            $ENV{DXSDK_DIR}/Lib/x86
            $ENV{DXSDK_DIR}/Lib/x64
            "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x86"
            "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64"
            "C:/Program Files/Microsoft DirectX SDK (June 2010)/Lib/x86"
            "C:/Program Files/Microsoft DirectX SDK (June 2010)/Lib/x64"
        DOC "DirectX 9 library"
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(DirectX9
        REQUIRED_VARS
            DirectX9_INCLUDE_DIR
            DirectX9_LIBRARY
    )

    mark_as_advanced(DirectX9_INCLUDE_DIR DirectX9_LIBRARY)
endif()

if(DirectX9_FOUND)
    set(DirectX9_INCLUDE_DIRS ${DirectX9_INCLUDE_DIR})
    set(DirectX9_LIBRARIES ${DirectX9_LIBRARY})
endif()
