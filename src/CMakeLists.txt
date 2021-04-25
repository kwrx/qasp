file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

add_executable(qasp ${SOURCES} ${HEADERS})

if(${HAVE_WASP})
    target_link_libraries(qasp wasp)
endif()

target_compile_options(qasp 
    PRIVATE -include "${PROJECT_BINARY_DIR}/config.h")