target_include_directories(mathiu PUBLIC
  ${PROJECT_SOURCE_DIR}/include PRIVATE matchit)

target_sources(mathiu PRIVATE core.cpp diff.cpp function_range.cpp inequation.cpp polynomial.cpp solve.cpp)

target_compile_options(mathiu PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(mathiu PRIVATE matchit)

set_target_properties(mathiu PROPERTIES CXX_EXTENSIONS OFF)
