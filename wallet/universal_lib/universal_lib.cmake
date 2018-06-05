cmake_minimum_required(VERSION 3.1.0)

if (NOT TARGET universal_lib)
    add_library(universal_lib STATIC IMPORTED GLOBAL)

    set_property(TARGET universal_lib PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/include"
        "${CMAKE_CURRENT_LIST_DIR}/include/WTL80")

    if (x64)
        set(UNIVERSAL_LIB_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/lib64)
    else()
        set(UNIVERSAL_LIB_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
    endif()

    set_property(TARGET universal_lib PROPERTY IMPORTED_LOCATION_DEBUG
        "${UNIVERSAL_LIB_LIBRARY_DIR}/CommonLib_Debug.lib")
    set_property(TARGET universal_lib PROPERTY IMPORTED_LOCATION_RELEASE
        "${UNIVERSAL_LIB_LIBRARY_DIR}/CommonLib.lib")
    set_property(TARGET universal_lib PROPERTY INTERFACE_LINK_LIBRARIES "$<$<CONFIG:Debug>:${UNIVERSAL_LIB_LIBRARY_DIR}/UILite_Debug.lib>" "$<$<CONFIG:Release>:${UNIVERSAL_LIB_LIBRARY_DIR}/UILite.lib>" "gdiplus" "psapi")
endif()