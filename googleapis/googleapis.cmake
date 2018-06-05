cmake_minimum_required(VERSION 3.1.0)

if (NOT TARGET googleapis)
    SET(OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/gen)
    SET(GOOGLE_PROTOBUF "../protobuf/src")
    add_definitions(-D_WIN32_WINNT=0x0601)

    FILE(GLOB_RECURSE GOOGLE_PROTO_INPUT ${CMAKE_CURRENT_LIST_DIR}/*.proto)
    SET(PROTOC "$<TARGET_FILE:protoc>")
    SET(PROTOC_C_OUT_FLAG --cpp_out)
    MESSAGE(STATUS "PROTOC = ${PROTOC}")

    FOREACH(PROTO_FILE ${GOOGLE_PROTO_INPUT})
      GET_FILENAME_COMPONENT(PROTO_NAME ${PROTO_FILE} NAME_WE)
     
      FILE(RELATIVE_PATH RELATIVE_PATH_NAME ${CMAKE_CURRENT_LIST_DIR} ${PROTO_FILE})
      GET_FILENAME_COMPONENT(DIRECTORY_NAME ${RELATIVE_PATH_NAME} DIRECTORY)
      
      SET(GOOGLEAPIS_CUR_PROTO_GEN
         ${OUTPUT_DIR}/${DIRECTORY_NAME}/${PROTO_NAME}.pb.h
         ${OUTPUT_DIR}/${DIRECTORY_NAME}/${PROTO_NAME}.pb.cc
      )
      
      SET(GOOGLEAPIS_PROTO_GEN_FILES
          ${GOOGLEAPIS_PROTO_GEN_FILES}
          ${GOOGLEAPIS_CUR_PROTO_GEN}
      )

      # MESSAGE(STATUS "CUR_PROTO_GEN = ${GOOGLEAPIS_CUR_PROTO_GEN}")
      
      ADD_CUSTOM_COMMAND(
        OUTPUT ${GOOGLEAPIS_CUR_PROTO_GEN}
        COMMAND ${PROTOC} ${PROTOC_C_OUT_FLAG}=${OUTPUT_DIR} -I ${CMAKE_CURRENT_LIST_DIR} --proto_path=${GOOGLE_PROTOBUF} ${PROTO_FILE}
        DEPENDS ${PROTOC} ${PROTO_FILE}
        BYPRODUCTS ${PROTO_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      )
     
    endforeach(PROTO_FILE ${GOOGLE_PROTO_INPUT})
    add_library(googleapis STATIC ${GOOGLEAPIS_PROTO_GEN_FILES})
    add_dependencies(googleapis protoc)
    target_include_directories(googleapis PRIVATE ${OUTPUT_DIR})
    target_link_libraries(googleapis libprotobuf)
endif()