cmake_minimum_required(VERSION 3.1.0)

if (NOT TARGET grpc_proto_gen)
    SET(PROTO_ROOT ${CMAKE_CURRENT_LIST_DIR})
    SET(GOOGLE_PROTOBUF "../protobuf/src")
    SET(GOOGLEAPIS_PROTOBUF "../googleapis")
    SET(GOOGLEAPIS_INCLUDE  "${CMAKE_CURRENT_LIST_DIR}/../googleapis/gen")
    SET(TRON_PROTOBUF ${CMAKE_CURRENT_LIST_DIR}/tronprotocol)
    add_definitions(-D_WIN32_WINNT=0x0601)
if (WIN32)
    SET(gRPC_CPP_PLUGIN_EXECUTABLE "../grpc/grpc_cpp_plugin.exe")
elseif (UNIX)
    find_program(gRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
endif()
    FILE(GLOB_RECURSE PROTO_INPUT ${PROTO_ROOT}/*.proto)
    SET(PROTOC "$<TARGET_FILE:protoc>")
    SET(PROTOC_C_OUT_FLAG --cpp_out)
    MESSAGE(STATUS "PROTOC = ${PROTOC}")

    FOREACH(PROTO_FILE ${PROTO_INPUT})
      GET_FILENAME_COMPONENT(PROTO_NAME ${PROTO_FILE} NAME_WE)
            
      GET_FILENAME_COMPONENT(DIRECTORY_NAME ${PROTO_FILE} DIRECTORY )   
      
      SET(CUR_PROTO_GEN
         ${DIRECTORY_NAME}/${PROTO_NAME}.pb.h
         ${DIRECTORY_NAME}/${PROTO_NAME}.pb.cc
      )
      
      SET(PROTO_GEN_FILES
          ${PROTO_GEN_FILES}
          ${CUR_PROTO_GEN}
      )

      # MESSAGE(STATUS "CUR_PROTO_GEN = ${CUR_PROTO_GEN}")
      
      ADD_CUSTOM_COMMAND(
        OUTPUT ${CUR_PROTO_GEN}
        COMMAND ${PROTOC} ${PROTOC_C_OUT_FLAG} ${TRON_PROTOBUF} -I${TRON_PROTOBUF} --proto_path=${GOOGLE_PROTOBUF} --proto_path=${GOOGLEAPIS_PROTOBUF} ${PROTO_FILE}
        DEPENDS ${PROTOC} ${PROTO_FILE}
        WORKING_DIRECTORY ${PROTO_ROOT}
      )
      
      SET(GRPC_CUR_PROTO_GEN
         ${DIRECTORY_NAME}/${PROTO_NAME}.grpc.pb.h
         ${DIRECTORY_NAME}/${PROTO_NAME}.grpc.pb.cc)
         
         
      SET(GRPC_PROTO_GEN_FILES
          ${GRPC_PROTO_GEN_FILES}
          ${GRPC_CUR_PROTO_GEN}
      )

      GET_FILENAME_COMPONENT(absolute_proto ${PROTO_FILE} ABSOLUTE)
      
      # MESSAGE(STATUS "absolute = ${absolute_proto}")
      
      ADD_CUSTOM_COMMAND(
          OUTPUT ${GRPC_CUR_PROTO_GEN}
          COMMAND ${PROTOC}
          ARGS --grpc_out "${TRON_PROTOBUF}" -I "${TRON_PROTOBUF}"
          --proto_path=${GOOGLE_PROTOBUF} --proto_path=${GOOGLEAPIS_PROTOBUF}
          --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}" "${absolute_proto}"
          DEPENDS "${absolute_proto}"
          WORKING_DIRECTORY ${PROTO_ROOT}
        )
        
    endforeach(PROTO_FILE ${PROTO_INPUT})
    
    add_library(grpc_proto_gen STATIC ${PROTO_GEN_FILES} ${GRPC_PROTO_GEN_FILES})
    add_dependencies(grpc_proto_gen protoc)
    
    target_include_directories(grpc_proto_gen PUBLIC ${PROTO_ROOT} ${TRON_PROTOBUF} ${GOOGLEAPIS_INCLUDE})
    
    target_link_libraries(grpc_proto_gen libprotobuf grpc_lib googleapis)
endif()