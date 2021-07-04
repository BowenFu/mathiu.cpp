include(FetchGTest)

message(STATUS "test CXX_STANDARD b : ${CMAKE_CXX_STANDARD}")

include(GoogleTest)

message(STATUS "test CXX_STANDARD c : ${CMAKE_CXX_STANDARD}")

message(STATUS "test CXX_STANDARD d : ${CMAKE_CXX_STANDARD}")

add_subdirectory(mathiu)

message(STATUS "test CXX_STANDARD e : ${CMAKE_CXX_STANDARD}")