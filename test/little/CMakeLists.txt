file(GLOB SOURCES "*.asp")

foreach(SOURCE ${SOURCES})

    cmake_path(GET SOURCE STEM TEST_NAME)
    cmake_path(GET SOURCE PARENT_PATH TEST_PATH)

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME}"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME} --look-ahead"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} -l | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME} --counter-example"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} -c | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME} --counter-example --look-ahead"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} -l -c | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

endforeach()




