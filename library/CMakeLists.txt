add_library(library INTERFACE)

target_include_directories(
  library
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

