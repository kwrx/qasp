file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

add_library(qasp ${SOURCES} ${HEADERS})
add_executable(qaspcli main.cc)


if(${HAVE_WASP})
    target_link_libraries(qaspcli qasp wasp)
endif()

target_compile_options(qasp 
    PRIVATE -include "${PROJECT_BINARY_DIR}/config.h")

target_compile_options(qaspcli
    PRIVATE -include "${PROJECT_BINARY_DIR}/config.h")

set_target_properties(qaspcli 
    PROPERTIES OUTPUT_NAME "qasp")
