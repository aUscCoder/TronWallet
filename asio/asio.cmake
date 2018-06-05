cmake_minimum_required(VERSION 3.1.0)

if(NOT TARGET asio)
    add_library(asio INTERFACE)

    target_include_directories(asio INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
endif()