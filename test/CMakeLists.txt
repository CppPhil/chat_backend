include(GoogleTest)

add_executable(
  unit_tests
  client_list_message_test.cpp
  contains_test.cpp
  main.cpp
  net_string_test.cpp)

target_link_libraries(
  unit_tests
  PRIVATE
  gtest
  library)

gtest_discover_tests(
  unit_tests
  WORKING_DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR})
