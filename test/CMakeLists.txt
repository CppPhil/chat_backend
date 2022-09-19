include(CTest)

add_executable(
  unit_tests
  client_list_message_test.cpp
  contains_test.cpp
  main.cpp
  net_string_test.cpp)

target_link_libraries(
  unit_tests
  PRIVATE
  library)

target_include_directories(
  unit_tests
  PRIVATE
  ${backend_SOURCE_DIR}/external/doctest/doctest)

add_test(
  NAME unit_tests
  COMMAND $<TARGET_FILE:unit_tests>)

