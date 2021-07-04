include(FetchContent)

FetchContent_Declare(
    matchit
    GIT_REPOSITORY https://github.com/BowenFu/matchit.cpp.git
    GIT_TAG main)

FetchContent_GetProperties(matchit)
if(NOT matchit_POPULATED)
    FetchContent_Populate(matchit)
    add_subdirectory(${matchit_SOURCE_DIR} ${matchit_BINARY_DIR}
                    EXCLUDE_FROM_ALL)
endif()

message(STATUS "Matchit header are present at ${matchit_SOURCE_DIR}")