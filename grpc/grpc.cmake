cmake_minimum_required(VERSION 3.1.0)

if (NOT TARGET grpc_lib)
  if (WIN32)
    add_library(grpc_lib STATIC IMPORTED GLOBAL)
    set_property(TARGET grpc_lib PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/include")
        
    set(GRPC_LIBRARY_DEBUG_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/Debug")
    set(GRPC_LIBRARY_RELEASE_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/Release")
        
    set_property(TARGET grpc_lib PROPERTY IMPORTED_LOCATION_DEBUG
        "${GRPC_LIBRARY_DEBUG_DIR}/grpc.lib")
    set_property(TARGET grpc_lib PROPERTY IMPORTED_LOCATION_RELEASE
        "${GRPC_LIBRARY_RELEASE_DIR}/grpc.lib")
        
    file(GLOB DEBUG_LIBS "${GRPC_LIBRARY_DEBUG_DIR}/*.lib")
    file(GLOB RELEASE_LIBS "${GRPC_LIBRARY_RELEASE_DIR}/*.lib")
    
    set_property(TARGET grpc_lib PROPERTY INTERFACE_COMPILE_DEFINITIONS _WIN32_WINNT=0x0601)
    
    set_property(TARGET grpc_lib PROPERTY INTERFACE_LINK_LIBRARIES "$<$<CONFIG:Debug>:${DEBUG_LIBS}>" "$<$<CONFIG:Release>:${RELEASE_LIBS}>")
  elseif (UNIX)
    find_path(GRPC_INCLUDE_DIR grpc/grpc.h)
    find_library(GRPC_LIBRARY NAMES grpc)
    find_library(GRPCPP_LIBRARY NAMES grpc++)
    find_library(GPR_LIBRARY NAMES gpr)
    
    add_library(grpc_lib STATIC IMPORTED GLOBAL)
    set_property(TARGET grpc_lib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR})
    set_property(TARGET grpc_lib PROPERTY IMPORTED_LOCATION ${GRPC_LIBRARY})
    set_property(TARGET grpc_lib PROPERTY INTERFACE_LINK_LIBRARIES ${GRPCPP_LIBRARY} ${GPR_LIBRARY})
  endif()
endif()