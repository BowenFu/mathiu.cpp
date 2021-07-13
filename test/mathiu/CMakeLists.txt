add_executable(unittests
expr.cpp order.cpp auto-simplification.cpp diff.cpp
solve.cpp polynomial.cpp relational.cpp freeOf.cpp
substitute.cpp functionRange.cpp inequation.cpp
)
target_compile_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(unittests PRIVATE mathiu gtest_main matchit)
set_target_properties(unittests PROPERTIES CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS OFF)
gtest_discover_tests(unittests)