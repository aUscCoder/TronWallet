cmake_minimum_required(VERSION 3.1.0)

project (base)

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++11 support has been enabled by default.")

if (WIN32)
    # disable minmax in windef.h
    ADD_DEFINITIONS(-DNOMINMAX)
endif()

file(GLOB CC_FILES *.cc *.h time/*.cc time/*.h numerics/*.h)
source_group("base" FILES ${CC_FILES})

file(GLOB NSPR_FILES third_party/nspr/*.cc third_party/nspr/*.h)
source_group("nspr" FILES ${NSPR_FILES})

if (WIN32)
    file(GLOB PLAT_CC_FILES win/*.cc win/*.h)
    source_group("win" FILES ${PLAT_CC_FILES})
elseif(APPLE)
    file(GLOB PLAT_CC_FILES mac/*.mm mac/*.h mac/*.cc posix/*.cc posix/*.h)
    list(REMOVE_ITEM PLAT_CC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/posix/sys_string_conversions_posix.cc ${CMAKE_CURRENT_SOURCE_DIR}/posix/time_now_posix.cc ${CMAKE_CURRENT_SOURCE_DIR}/posix/waitable_event_posix.cc)
    source_group("mac" FILES ${PLAT_CC_FILES})
elseif(ANDROID)
    file(GLOB PLAT_CC_FILES android/*.cc android/*.h posix/*.cc posix/*.h)
    list(REMOVE_ITEM PLAT_CC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/posix/sys_string_conversions_posix.cc)
    include("android/jni_generator/jni_generator.cmake")
    file(GLOB JNI_INPUT "../../java/com/typany/base/SystemMessageHandler.java")
    GenerateJNIFiles("${JNI_INPUT}" "jni" "" "JNI_OUTPUT_FILES")
    list(APPEND PLAT_CC_FILES ${JNI_OUTPUT_FILES})
    source_group("android" FILES ${PLAT_CC_FILES})
elseif(UNIX)
    file(GLOB PLAT_CC_FILES posix/*.cc posix/*.h)
    source_group("posix" FILES ${PLAT_CC_FILES})
endif()

file(GLOB MESSAGE_LOOP_FILES message_loop/*.cc message_loop/*.h)
source_group("message_loop" FILES ${MESSAGE_LOOP_FILES})

file(GLOB THREADING_FILES threading/*.cc threading/*.h)
source_group("threading" FILES ${THREADING_FILES})

file(GLOB SYNCHRONIZATION_FILES synchronization/*.cc synchronization/*.h)
source_group("synchronization" FILES ${SYNCHRONIZATION_FILES})

if (USE_ICU)
    file(GLOB I18N_FILES FILES i18n/*.cc i18n/*.h)
    source_group("i18n" FILES ${I18N_FILES})
    list(APPEND CC_FILES ${I18N_FILES})
endif()

add_library(${PROJECT_NAME} STATIC ${CC_FILES} ${PLAT_CC_FILES} ${NSPR_FILES} ${MESSAGE_LOOP_FILES} ${THREADING_FILES} ${SYNCHRONIZATION_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/.." PUBLIC "${PROJECT_SOURCE_DIR}/third_party/icu")
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "infra")

if (WIN32)
    list(APPEND DEPS "Winmm.lib")
else ()
    add_subdirectory("third_party/libevent")
    list(APPEND DEPS "libevent")
endif()

if (USE_ICU)
    add_subdirectory("../icu" "../icu")
    list(APPEND DEPS "icu_i18n" "icu_uc")
endif()

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework Foundation" "-framework CoreFoundation" "-framework Cocoa")
endif()

if (USE_LIBXML)
    include("../libxml/libxml.cmake")
    ADD_DEFINITIONS(-DUSE_LIBXML)
    list(APPEND DEPS "libxml")
endif()

if (USE_BASE64)
    include("../modp_b64/modp_b64.cmake")
    ADD_DEFINITIONS(-DUSE_BASE64)
    list(APPEND DEPS "modp_b64")
endif()

target_link_libraries(${PROJECT_NAME} ${DEPS})

target_compile_options(${PROJECT_NAME} PUBLIC "$<$<C_COMPILER_ID:MSVC>:/utf-8>")
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (ENABLE_CONTAINER_TEST)
    add_executable(container_test container_test.cc)
    target_link_libraries(container_test ${PROJECT_NAME})
endif()
