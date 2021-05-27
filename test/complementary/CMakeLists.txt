file(GLOB SOURCES "*.asp")

foreach(SOURCE ${SOURCES})

    cmake_path(GET SOURCE STEM TEST_NAME)
    cmake_path(GET SOURCE PARENT_PATH TEST_PATH)

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME}"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp -c ${SOURCE} | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )


    add_test(
        NAME "[STDIN] ${TEST_PATH}-${TEST_NAME}"
        COMMAND /bin/sh -c "cat ${SOURCE} | ${PROJECT_BINARY_DIR}/bin/qasp -c | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

endforeach()




