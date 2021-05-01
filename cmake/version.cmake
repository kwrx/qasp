set(CMAKE_PROJECT_NAME "qasp")
set(CMAKE_PROJECT_VERSION_MAJOR "1")
set(CMAKE_PROJECT_VERSION_MINOR "0")
set(CMAKE_PROJECT_VERSION_PATCH "0-alpha")
set(CMAKE_PROJECT_VERSION_TWEAK "1")
set(CMAKE_PROJECT_DESCRIPTION "Solving quantifier over ASP")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/kwrx/qasp")


string(TOUPPER 
    ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_UPPER)

set(CMAKE_PROJECT_GREETINGS "${CMAKE_PROJECT_NAME_UPPER} ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}")