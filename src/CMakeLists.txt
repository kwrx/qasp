file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")
file(GLOB_RECURSE PUBHDRS "${CMAKE_SOURCE_DIR}/include/qasp/qasp.*")

add_library(qasp ${SOURCES} ${HEADERS})
add_executable(qaspcli main.cc)

target_link_libraries(qaspcli
    PRIVATE qasp)


if(${HAVE_WASP})
    target_link_libraries(qasp
        PRIVATE wasp)
endif()

if(${HAVE_GRINGO})

    target_include_directories(qasp 
        PRIVATE 
            "${CMAKE_SOURCE_DIR}/lib/clingo/libclingo"
            "${CMAKE_SOURCE_DIR}/lib/clingo/libgringo"
            "${CMAKE_SOURCE_DIR}/lib/clingo/libreify"
            "${CMAKE_SOURCE_DIR}/lib/clingo/clasp"
            "${CMAKE_BINARY_DIR}/lib/clingo/clasp"
            "${CMAKE_SOURCE_DIR}/lib/clingo/clasp/libpotassco")

    target_link_libraries(qasp
        PRIVATE
            "${CMAKE_BINARY_DIR}/lib/clingo/lib/libclingo.a"
            "${CMAKE_BINARY_DIR}/lib/clingo/lib/libgringo.a"
            "${CMAKE_BINARY_DIR}/lib/clingo/lib/libclasp.a"
            "${CMAKE_BINARY_DIR}/lib/clingo/lib/libreify.a"
            "${CMAKE_BINARY_DIR}/lib/clingo/lib/libpotassco.a")

endif()


target_compile_options(qasp 
    PRIVATE -include "${PROJECT_BINARY_DIR}/config.h")

target_compile_options(qaspcli
    PRIVATE -include "${PROJECT_BINARY_DIR}/config.h")

set_target_properties(qaspcli 
    PROPERTIES OUTPUT_NAME "qasp")

set_target_properties(qaspcli
    PROPERTIES PUBLIC_HEADER "${PUBHDRS}")