if(NOT ANDROID)
  if(NOT UNIX)
    return()
  endif()
endif()

include(CheckCXXSourceCompiles)
include(CheckLibraryExists)

if(WITH_JEMALLOC)
  find_package(Jemalloc REQUIRED)
  if(NOT JEMALLOC_FOUND)
    message(FATAL_ERROR "did not find jemalloc")
  endif()
  add_definitions(-DUSE_JEMALLOC)
  message(STATUS "using jemalloc")
endif()

add_definitions(-DUNIX)
add_definitions(-DPOSIX)

if (STATIC_LINK_RUNTIME OR STATIC_LINK)
  set(LIBUV_USE_STATIC ON)
endif()


option(DOWNLOAD_UV "statically compile in libuv" OFF)
# Allow -DDOWNLOAD_UV=FORCE to download without even checking for a local libuv
if(NOT DOWNLOAD_UV STREQUAL "FORCE")
  find_package(LibUV 1.28.0)
endif()
if(LibUV_FOUND)
  message(STATUS "using system libuv")
elseif(DOWNLOAD_UV)
  message(STATUS "using libuv submodule")
  set(LIBUV_ROOT ${CMAKE_SOURCE_DIR}/external/libuv)
  add_subdirectory(${LIBUV_ROOT})
  set(LIBUV_INCLUDE_DIRS ${LIBUV_ROOT}/include)
  set(LIBUV_LIBRARY uv_a)
  if(NOT ANDROID)
    add_definitions(-D_LARGEFILE_SOURCE)
    add_definitions(-D_FILE_OFFSET_BITS=64)
  endif()
endif()
include_directories(${LIBUV_INCLUDE_DIRS})

if(EMBEDDED_CFG OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  link_libatomic()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
  add_definitions(-D_BSD_SOURCE)
  add_definitions(-D_GNU_SOURCE)
  add_definitions(-D_XOPEN_SOURCE=700)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  if (LIBUV_USE_STATIC)
    link_libraries(-lkstat -lsendfile)
  endif()
endif()
