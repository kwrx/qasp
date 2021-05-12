file(GLOB_RECURSE SOURCES "*.asp")

foreach(SOURCE ${SOURCES})

    cmake_path(GET SOURCE STEM TEST_NAME)
    cmake_path(GET SOURCE PARENT_PATH TEST_PATH)

    add_test(
        NAME "[FILE]  ${TEST_NAME}"
        COMMAND /bin/sh -c "diff -u ${TEST_PATH}/${TEST_NAME}.expected <(${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE})"
        WORKING_DIRECTORY .
    )


    add_test(
        NAME "[STDIN] ${TEST_NAME}"
        COMMAND /bin/sh -c "diff -u ${TEST_PATH}/${TEST_NAME}.expected <(cat ${SOURCE} | ${PROJECT_BINARY_DIR}/bin/qasp)"
        WORKING_DIRECTORY .
    )

endforeach()




