cmake_minimum_required(VERSION 3.5.1)

set(ETH_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${ETH_CMAKE_DIR})

project(TronWallet)

set(compiler_flags_overrides "${CMAKE_SOURCE_DIR}/cmake/compiler_flags_overrides.cmake")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4800 /wd4100 /wd4125 /wd4458")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
   if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)

include(ProjectSecp256k1)
include(ProjectLibFF)
include("${CMAKE_SOURCE_DIR}/grpc/grpc.cmake")
include("${CMAKE_SOURCE_DIR}/grpc_proto/grpc_proto_gen.cmake")
include("${CMAKE_SOURCE_DIR}/googleapis/googleapis.cmake")
include("${CMAKE_SOURCE_DIR}/boost_1_65_1/boost_lib.cmake")

#Global include path for all libs.
include_directories("${CMAKE_SOURCE_DIR}")

set(UTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/utils")
# protobuf options
set(protobuf_VERBOSE OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(protobuf_MSVC_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
set(protobuf_UNICODE ON CACHE BOOL "" FORCE)

add_subdirectory(base)
add_subdirectory(cryptopp)
add_subdirectory(jsoncpp)
add_subdirectory("protobuf/cmake")
add_subdirectory(libscrypt)
add_subdirectory(libdevcore)
add_subdirectory(libdevcrypto)
add_subdirectory(wallet)


