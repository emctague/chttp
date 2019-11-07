cmake_minimum_required(VERSION 3.15)
project(CHTTP VERSION 0.0.1 LANGUAGES C)
include(CMakePackageConfigHelpers)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(CHTTP
  src/HTTP.c include/chttp/HTTP.h
  src/Socket.c include/chttp/Socket.h
  src/StringMap.c include/chttp/StringMap.h
  src/Server.c include/chttp/Server.h
  src/Logger.c include/chttp/Logger.h
  include/chttp/Error.h)

add_library(CHTTP::CHTTP ALIAS CHTTP)

target_link_libraries(CHTTP PRIVATE Threads::Threads)
target_include_directories(CHTTP
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/CHTTP)

install(TARGETS CHTTP
  EXPORT chttp-export
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT chttp-export
  FILE
    CHTTPTargets.cmake
  NAMESPACE
    CHTTP::
  DESTINATION
  ${INSTALL_CONFIGDIR})

export(PACKAGE CHTTP)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/CHTTPConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)


configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/CHTTPConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CHTTPConfig.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/CHTTPConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/CHTTPConfigVersion.cmake
  DESTINATION ${INSTALL_CONFIGDIR})

# Simple test executable
add_executable(test test.c)
target_link_libraries(test CHTTP)
