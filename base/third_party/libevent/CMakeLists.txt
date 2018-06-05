cmake_minimum_required(VERSION 3.1.0)

project (libevent)

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++11 support has been enabled by default.")

set(CC_FILES "buffer.c"
             "evbuffer.c"
             "evdns.c"
             "evdns.h"
             "event-internal.h"
             "event.c"
             "event.h"
             "event_tagging.c"
             "evhttp.h"
             "evrpc-internal.h"
             "evrpc.c"
             "evrpc.h"
             "evsignal.h"
             "evutil.c"
             "evutil.h"
             "http-internal.h"
             "http.c"
             "log.c"
             "log.h"
             "min_heap.h"
             "poll.c"
             "select.c"
             "signal.c"
             "strlcpy-internal.h"
             "strlcpy.c")
source_group("libevent" FILES ${CC_FILES})

if (APPLE OR IOS)
    set(PLAT_CC_FILES "kqueue.c" "mac/config.h" "mac/event-config.h")
    list(APPEND CC_FILES ${PLAT_CC_FILES})
    source_group("mac" FILES ${PLAT_CC_FILES})
    set(PLATFORM_INCLUDE "mac")
elseif (ANDROID)
    set(PLAT_CC_FILES "epoll.c" "android/config.h" "android/event-config.h")
    list(APPEND CC_FILES ${PLAT_CC_FILES})
    source_group("android" FILES ${PLAT_CC_FILES})
    set(PLATFORM_INCLUDE "android")
elseif (UNIX)
    set(PLAT_CC_FILES "epoll.c" "linux/config.h" "linux/event-config.h")
    list(APPEND CC_FILES ${PLAT_CC_FILES})
    source_group("linux" FILES ${PLAT_CC_FILES})
    set(PLATFORM_INCLUDE "linux")
endif()

add_library(${PROJECT_NAME} STATIC ${CC_FILES})

target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_CONFIG_H)
target_include_directories(${PROJECT_NAME} PUBLIC ${PLATFORM_INCLUDE})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "infra")
