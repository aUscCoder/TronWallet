cmake_minimum_required(VERSION 3.1.0)
project (wallet)

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_DEFINITIONS(-D_UNICODE)

FILE(GLOB_RECURSE GUI_FILES gui/*.cpp gui/*.h)
FILE(GLOB_RECURSE CORE_FILES core/*.cpp core/*.h)
FILE(GLOB_RECURSE COMMONUI_FILES common_ui/*.cpp common_ui/*.h)
FILE(GLOB_RECURSE DUMP_FILES dump/*.cpp dump/*.h)
FILE(GLOB_RECURSE QRCODE_FILES QRCode/*.cpp QRCode/*.h QRCode/*.c)

set(CC_FILES main.cpp wallet_rc.h wallet.rc)

set(RESOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/btn_normal.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/btn_cancel.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/btn_close.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/btn_max.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/btn_min.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/btn_restore.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/shadow.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/dropdown_triangle.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/radio.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/scrollv.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/std_edit.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/std_combo_bg.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/eye_normal.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/eye_select.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/logo.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/index.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/seperator.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/loading.gif
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/tron.ico
)

source_group("gui" FILES ${GUI_FILES})
source_group("core" FILES ${CORE_FILES})
source_group("resources" FILES ${RESOURCE_FILES})
source_group("common_ui" FILES ${COMMONUI_FILES})
source_group("dump" FILES ${DUMP_FILES})


include_directories(${CMAKE_CURRENT_SOURCE_DIR} common_ui universal_lib/include universal_lib/include/WTL80)
include(universal_lib/universal_lib.cmake)

add_executable(${PROJECT_NAME} ${CC_FILES} ${GUI_FILES} ${CORE_FILES} ${COMMONUI_FILES} ${RESOURCE_FILES} ${DUMP_FILES} ${QRCODE_FILES})
target_link_libraries(${PROJECT_NAME} base devcrypto grpc_proto_gen universal_lib gdiplus Dbghelp)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)