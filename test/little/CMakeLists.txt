file(GLOB SOURCES "*.qasp")

foreach(SOURCE ${SOURCES})

    cmake_path(GET SOURCE STEM TEST_NAME)
    cmake_path(GET SOURCE PARENT_PATH TEST_PATH)

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME}"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} -n0 | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME} --look-ahead"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} -n0 -l | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME} --counter-example"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} -n0 -c | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME} --counter-example --look-ahead"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE} -n0 -l -c | diff -s ${TEST_PATH}/${TEST_NAME}.expected -"
        WORKING_DIRECTORY .
    )

endforeach()




