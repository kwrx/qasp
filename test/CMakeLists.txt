file(GLOB_RECURSE SOURCES "*.asp")
list(GET ${SOURCES} 0 FIRST_SOURCE)


foreach(SOURCE ${SOURCES})

    get_filename_component(TEST ${SOURCE} NAME_WE)

    add_test(
        NAME ${TEST}
        COMMAND ${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE}
        WORKING_DIRECTORY .
    )

endforeach()


add_test(
    NAME "ReadingFromStdin"
    COMMAND /bin/sh -c "cat ${FIRST_SOURCE} | ${PROJECT_BINARY_DIR}/bin/qasp"
    WORKING_DIRECTORY .
)


