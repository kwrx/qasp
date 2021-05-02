if(${HAVE_THREADS})
    find_package(Threads REQUIRED)
endif()

if(${HAVE_WASP})

    find_package(Boost REQUIRED)

    if(Boost_FOUND)
        add_subdirectory(lib/wasp)
    endif()
    
endif()

if(${HAVE_CLASP})
    message(FATAL_ERROR "clasp not yet supported")
endif()

if(${HAVE_GRINGO})
    find_program(GRINGO gringo REQUIRED)
endif()

if(${HAVE_IDLV})
    find_program(IDLV idlv REQUIRED)
endif()