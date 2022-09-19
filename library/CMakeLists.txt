add_library(library INTERFACE)

target_include_directories(
  library
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  library
  INTERFACE
  Poco::Net
  Poco::JSON
  CppPhil::philslib)
