if(${HAVE_THREADS})
    find_package(Threads REQUIRED)
endif()



##
## Unical
##

if(${HAVE_IDLV})
    find_program(IDLV idlv REQUIRED)
endif()

if(${HAVE_WASP})

    find_package(Boost REQUIRED)

    if(Boost_FOUND)
        add_subdirectory(lib/wasp)
    endif()
    
endif()


##
## Potassco
##

if(${HAVE_GRINGO_EXTERNAL})
    find_program(IDLV gringo REQUIRED)
endif()

if(${HAVE_GRINGO} OR ${HAVE_CLASP})

    if(!${HAVE_THREADS})
        message(FATAL_ERROR "HAVE_THREADS must be ON to use gringo/clasp")
    endif()

    find_package(Threads REQUIRED)
    find_package(BISON REQUIRED)
    find_program(RE2C "re2c" REQUIRED)

    include(ExternalProject)

    ExternalProject_Add(clingo
        SOURCE_DIR          "${CMAKE_SOURCE_DIR}/lib/clingo"
        BINARY_DIR          "${CMAKE_BINARY_DIR}/lib/clingo"
        INSTALL_DIR         "${CMAKE_BINARY_DIR}/lib/clingo/install"
        BUILD_ALWAYS        True
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/lib/clingo
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCLINGO_BUILD_APPS=OFF
            -DCLINGO_BUILD_STATIC=ON
            -DCLINGO_BUILD_WITH_PYTHON=OFF
            -DCLINGO_BUILD_WITH_LUA=OFF)

endif()