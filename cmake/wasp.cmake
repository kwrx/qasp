
if(${HAVE_WASP})

    find_package(Boost REQUIRED)

    if(Boost_FOUND)
        add_subdirectory(lib/wasp)
    endif()
    
endif()
