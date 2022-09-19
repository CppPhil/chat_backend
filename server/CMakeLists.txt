add_executable(
  server
  include/clients.hpp
  src/clients.cpp
  src/main.cpp)

target_link_libraries(server PRIVATE library fmt::fmt)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
