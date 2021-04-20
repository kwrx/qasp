file(GLOB_RECURSE SOURCES "*.asp")
list(GET ${SOURCES} 0 FIRST_SOURCE)

message("Test found: " ${SOURCES})

foreach(SOURCE ${SOURCES})

    get_filename_component(TEST ${SOURCE} NAME_WE)

    add_test(
        NAME ${TEST}
        CONFIGURATIONS Debug
        COMMAND ${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE}
        WORKING_DIRECTORY .
    )

endforeach()


add_test(
    NAME "ReadingFromStdin"
    CONFIGURATIONS Debug
    COMMAND /bin/sh -c "cat ${FIRST_SOURCE} | ${PROJECT_BINARY_DIR}/bin/qasp"
    WORKING_DIRECTORY .
)


