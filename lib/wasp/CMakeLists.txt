file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

add_library(wasp ${SOURCES} ${HEADERS})


target_link_libraries(wasp PUBLIC ${Boost_LIBRARIES})
target_include_directories(wasp PUBLIC ${Boost_INCLUDE_DIRS})

target_compile_options(wasp PRIVATE -Wno-error -w)
target_compile_definitions(wasp PRIVATE __qasp__)