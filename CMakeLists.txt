cmake_minimum_required(VERSION 3.11)
project(QASP CXX)

set(QASP_PROGRAM_NAME "qasp")
set(QASP_VERSION_MAJOR "1")
set(QASP_VERSION_MINOR "0")
set(QASP_VERSION_PATCH "0-alpha.1")



set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -fprofile-arcs -ftest-coverage -include ${PROJECT_BINARY_DIR}/config.h")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer")
endif()


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG 1)
    set(DEBUG_LEVEL ALL)
endif()


set(HAVE_GRINGO 1)
set(HAVE_IDLV 0)


configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
