cmake_minimum_required(VERSION 3.20)
project(Fabada256 LANGUAGES C)

include(CTest)
option(FABADA_BUILD_TESTU01 "Build the TestU01 driver" OFF)
option(FABADA_BUILD_SMHASHER3 "Build the pinned SMHasher3 integration" OFF)

add_library(fabada256 src/fabada256.c)

target_include_directories(fabada256 PUBLIC include)
set_target_properties(fabada256 PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

add_executable(fabada256_cli src/main.c)
target_link_libraries(fabada256_cli PRIVATE fabada256)
set_target_properties(fabada256_cli PROPERTIES OUTPUT_NAME fabada256)

if(BUILD_TESTING)
  add_executable(test_fabada256 tests/test_fabada256.c)
  target_link_libraries(test_fabada256 PRIVATE fabada256)
  add_test(NAME unit COMMAND test_fabada256)
endif()

if(FABADA_BUILD_TESTU01)
  if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
  endif()
  
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  find_package(TestU01 REQUIRED)
  add_executable(test_testu01 tests/test_testu01.c)
  target_link_libraries(test_testu01 PRIVATE fabada256 TestU01::TestU01)
endif()

if(FABADA_BUILD_SMHASHER3)
  enable_language(CXX)
  include(cmake/SMHasher3.cmake)
endif()

