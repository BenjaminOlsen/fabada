# Included by the patched, separate upstream CMake project.
if(NOT EXISTS "${FABADA_ROOT}/src/fabada256.c")
  message(FATAL_ERROR "FABADA_ROOT must point to the standalone Fabada repository")
endif()
target_sources(SMHasher3Hashlib PRIVATE "${FABADA_ROOT}/src/fabada256.c")
target_include_directories(SMHasher3Hashlib PRIVATE "${FABADA_ROOT}/include")
set_source_files_properties("${FABADA_ROOT}/src/fabada256.c" PROPERTIES
  C_STANDARD 11 C_EXTENSIONS OFF)
add_executable(fabada_adapter_check "${FABADA_ROOT}/tests/smhasher3/adapter_check.cpp")
target_include_directories(fabada_adapter_check PRIVATE include/hashlib include/common
  "${FABADA_ROOT}/include")
target_link_libraries(fabada_adapter_check PRIVATE SMHasher3Hashlib SMHasher3Tests ${threads_link_command})
