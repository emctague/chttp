cmake_minimum_required(VERSION 3.15)
project(CHTTP)
include(CMakePackageConfigHelpers)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(CHTTP
  src/HTTP.c include/chttp/HTTP.h
  src/Socket.c include/chttp/Socket.h
  src/StringMap.c include/chttp/StringMap.h
  src/Server.c include/chttp/Server.h
  src/Logger.c include/chttp/Logger.h)

target_link_libraries(CHTTP PUBLIC Threads::Threads)
target_include_directories(CHTTP
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

install(TARGETS CHTTP
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

add_executable(test test.c)
target_link_libraries(test CHTTP)

