set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} -pipe")
    set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -ggdb -Wall -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -fomit-frame-pointer -fno-plt -D_FORTIFY_SOURCE=2 -DNDEBUG=1")

    if(ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage --coverage")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -lgcov --coverage")
    endif()

    if(ENABLE_PROFILING)
        set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} -pg")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -pg")
    endif()

    if(HAVE_THREADS)
        set(CMAKE_EXE_LINKER_FLAGS          "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE  "${CMAKE_EXE_LINKER_FLAGS} -s")
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_EXE_LINKER_FLAGS          "${CMAKE_EXE_LINKER_FLAGS} -lstdc++ -lc -lm")
endif()



if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG 1)
    set(DEBUG_LEVEL __ALL)
endif()


include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_compile_definitions(__qasp__)