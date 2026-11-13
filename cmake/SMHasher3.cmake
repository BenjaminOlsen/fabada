option(FABADA_NATIVE "Use native CPU optimization in SMHasher3" OFF)

find_package(Git REQUIRED)

include(ExternalProject)

set(_upstream "${PROJECT_SOURCE_DIR}/third_party/smhasher3")

if(NOT EXISTS "${_upstream}/CMakeLists.txt")
  message(FATAL_ERROR "Initialize SMHasher3 with: git submodule update --init --recursive")
endif()

set(_generated "${PROJECT_BINARY_DIR}/_deps/smhasher3-src")
set(_binary "${PROJECT_BINARY_DIR}/_deps/smhasher3-build")

# Reconfigure when owned integration inputs change; the actual library source
# is compiled directly by the external build, so source edits rebuild normally.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/tests/smhasher3/integration.patch"
  "${PROJECT_SOURCE_DIR}/tests/smhasher3/fabada256.cpp"
  "${PROJECT_SOURCE_DIR}/cmake/PrepareSMHasher3.cmake")

execute_process(COMMAND "${CMAKE_COMMAND}"
  "-DUPSTREAM=${_upstream}" "-DGENERATED=${_generated}"
  "-DFABADA_ROOT=${PROJECT_SOURCE_DIR}" "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
  -P "${PROJECT_SOURCE_DIR}/cmake/PrepareSMHasher3.cmake"
  RESULT_VARIABLE _status)

if(NOT _status EQUAL 0)
  message(FATAL_ERROR "SMHasher3 source preparation failed (see above)")
endif()

ExternalProject_Add(smhasher3
  SOURCE_DIR "${_generated}" BINARY_DIR "${_binary}"
  DOWNLOAD_COMMAND "" UPDATE_COMMAND "" INSTALL_COMMAND ""
  CMAKE_ARGS "-DFABADA_ROOT:PATH=${PROJECT_SOURCE_DIR}"
    "-DFABADA_NATIVE:BOOL=${FABADA_NATIVE}" "-DCMAKE_BUILD_TYPE:STRING=Release"
    "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
    "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
  BUILD_ALWAYS TRUE
  BUILD_BYPRODUCTS "${_binary}/SMHasher3${CMAKE_EXECUTABLE_SUFFIX}"
    "${_binary}/fabada_adapter_check${CMAKE_EXECUTABLE_SUFFIX}"
  USES_TERMINAL_CONFIGURE TRUE USES_TERMINAL_BUILD TRUE
)

# Separate commands keep statistical work out of builds and default CTest.
add_custom_target(smhasher3-adapter-check
  COMMAND "${_binary}/fabada_adapter_check${CMAKE_EXECUTABLE_SUFFIX}"
  DEPENDS smhasher3 USES_TERMINAL)

add_custom_target(smhasher3-quick
  COMMAND "${_binary}/SMHasher3${CMAKE_EXECUTABLE_SUFFIX}" --test=Avalanche,BIC Fabada256
  DEPENDS smhasher3 USES_TERMINAL)

add_custom_target(smhasher3-quality
  COMMAND "${_binary}/SMHasher3${CMAKE_EXECUTABLE_SUFFIX}"
    --test=Avalanche,BIC,Bitflip,Sparse,TwoBytes,Cyclic,Permutation,Zeroes,Text Fabada256
  DEPENDS smhasher3 USES_TERMINAL)

