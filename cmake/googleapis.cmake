include(ExternalProject)

# Build googleapis as an external project.
SET(PROTOC "$<TARGET_FILE:protoc>")
SET(gRPC_CPP_PLUGIN_EXECUTABLE "../grpc/grpc_cpp_plugin.exe")
set(GOOGLEAPIS_INSTALL_DIR ${CMAKE_SOURCE_DIR}/googleapis)
set(GOOGLEAPIS_INCLUDE_DIR ${GOOGLEAPIS_INSTALL_DIR}/include)
set(GOOGLEAPIS_LIB_DIR ${GOOGLEAPIS_INSTALL_DIR}/lib)
ExternalProject_Add(googleapis_external_project
                    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/googleapis
                    PREFIX      ${GOOGLEAPIS_INSTALL_DIR}
                    INSTALL_DIR ${GOOGLEAPIS_INSTALL_DIR}
                    CMAKE_ARGS  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                                -DCMAKE_CXX_FLAGS=${EXTERNAL_PROJECT_CMAKE_CXX_FLAGS}
                                -DCMAKE_INSTALL_PREFIX:PATH=${GOOGLEAPIS_INSTALL_DIR}
                                -DPROTOC=${PROTOC}
                                -DGRPC_CPP_PLUGIN=${GRPC_CPP_PLUGIN}
                   )
                   
include_directories(BEFORE SYSTEM ${GOOGLEAPIS_INCLUDE_DIR})
link_directories(${GOOGLEAPIS_LIB_DIR})