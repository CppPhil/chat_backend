cmake_minimum_required(VERSION 3.8)

enable_testing()

project(backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(ENABLE_ENCODINGS OFF CACHE BOOL "Enable Encodings" FORCE)
set(ENABLE_XML OFF CACHE BOOL "Enable XML" FORCE)
set(ENABLE_JSON ON CACHE BOOL "Enable JSON" FORCE)
set(ENABLE_MONGODB OFF CACHE BOOL "Enable MongoDB" FORCE)
set(ENABLE_DATA_SQLITE OFF CACHE BOOL "Enable Data SQlite" FORCE)
set(ENABLE_DATA_MYSQL OFF CACHE BOOL "Enable Data MySQL" FORCE)
set(ENABLE_DATA_POSTGRESQL OFF CACHE BOOL "Enable Data PostgreSQL" FORCE)
set(ENABLE_DATA_ODBC OFF CACHE BOOL "Enable Data ODBC" FORCE)
set(ENABLE_REDIS OFF CACHE BOOL "Enable Redis" FORCE)
set(ENABLE_ZIP OFF CACHE BOOL "Enable Zip" FORCE)
set(ENABLE_PAGECOMPILER OFF CACHE BOOL "Enable PageCompiler" FORCE)
set(ENABLE_PAGECOMPILER_FILE2PAGE OFF CACHE BOOL "Enable File2Page" FORCE)
set(ENABLE_ACTIVERECORD OFF CACHE BOOL "Enable ActiveRecord" FORCE)
set(ENABLE_ACTIVERECORD_COMPILER OFF CACHE BOOL "Enable ActiveRecord Compiler" FORCE)

add_subdirectory(external/poco)
add_subdirectory(external/fmt)
add_subdirectory(external/gsl)
add_subdirectory(external/philslib)

add_subdirectory(library)
add_subdirectory(server)
add_subdirectory(test)
