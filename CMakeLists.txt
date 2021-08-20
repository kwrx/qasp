cmake_minimum_required(VERSION 3.20)
project(QASP C CXX)

#
# Config options
#

## Grounder
set(HAVE_GRINGO                 1)
set(HAVE_GRINGO_EXTERNAL        0)
set(HAVE_IDLV                   0)

## Solver
set(HAVE_WASP                   1)
set(HAVE_CLASP                  0)

## QASP
set(HAVE_THREADS                1)
set(HAVE_CACHE                  1)
set(HAVE_MODE_LOOK_AHEAD        1)
set(HAVE_MODE_COUNTER_EXAMPLE   1)

## Wasp
set(HAVE_WASP_ASSUMPTIONS       1)
set(HAVE_WASP_RESET             1)

## Logger
set(HAVE_PRETTY_LOGGER          1)
set(HAVE_PERFORMANCE            0)

# Build Test Options
set(ENABLE_COVERAGE             0)
set(ENABLE_PROFILING            0)
set(ENABLE_NATIVE               0)
set(ENABLE_STATIC               1)

######################################



include(cmake/version.cmake)
include(cmake/package.cmake)
include(cmake/deps.cmake)
include(cmake/build.cmake)
include(CPack)
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
#add_subdirectory(test/big)
add_subdirectory(test/little)


install(TARGETS qaspcli qasp
    RUNTIME PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE
    ARCHIVE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE
    LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE
)