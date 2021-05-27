file(GLOB SOURCES "*.asp")

foreach(SOURCE ${SOURCES})

    cmake_path(GET SOURCE FILENAME TEST_NAME)
    cmake_path(GET SOURCE PARENT_PATH TEST_PATH)

    add_test(
        NAME "[FILE]  ${TEST_PATH}-${TEST_NAME}"
        COMMAND /bin/sh -c "${PROJECT_BINARY_DIR}/bin/qasp ${SOURCE}"
        WORKING_DIRECTORY .
    )

endforeach()




