include(FetchContent)

FetchContent_Declare(
    matchit
    GIT_REPOSITORY https://github.com/BowenFu/matchit.cpp.git
    GIT_TAG main)

FetchContent_MakeAvailable(matchit)

message(STATUS "Matchit header are present at ${matchit_SOURCE_DIR}")