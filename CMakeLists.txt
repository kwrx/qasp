cmake_minimum_required(VERSION 3.11)
project(QASP CXX)

#
# Config options
#
set(HAVE_GRINGO     1)
set(HAVE_IDLV       0)



include(cmake/version.cmake)
include(cmake/build.cmake)
include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})


configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
