set(TESTU01_ROOT "" CACHE PATH "Prefix containing TestU01 include/ and lib/")

find_path(TestU01_INCLUDE_DIR TestU01.h HINTS "${TESTU01_ROOT}" PATH_SUFFIXES include include/TestU01)

foreach(component testu01 probdist mylib)
  find_library(TestU01_${component}_LIBRARY NAMES ${component}
    HINTS "${TESTU01_ROOT}" PATH_SUFFIXES lib lib64)
endforeach()

# TestU01 relies on libm on Unix; the Windows CRT provides math itself.
if(UNIX)
  find_library(TestU01_MATH_LIBRARY NAMES m)
else()
  set(TestU01_MATH_LIBRARY "")
endif()

include(FindPackageHandleStandardArgs)
set(_required TestU01_INCLUDE_DIR TestU01_testu01_LIBRARY TestU01_probdist_LIBRARY TestU01_mylib_LIBRARY)

if(UNIX)
  list(APPEND _required TestU01_MATH_LIBRARY)
endif()

find_package_handle_standard_args(TestU01 REQUIRED_VARS ${_required}
  REASON_FAILURE_MESSAGE "Install TestU01 and set TESTU01_ROOT to its installation prefix, or disable FABADA_BUILD_TESTU01.")

if(TestU01_FOUND AND NOT TARGET TestU01::TestU01)
  add_library(TestU01::TestU01 INTERFACE IMPORTED)
  set_target_properties(TestU01::TestU01 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${TestU01_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${TestU01_testu01_LIBRARY};${TestU01_probdist_LIBRARY};${TestU01_mylib_LIBRARY};${TestU01_MATH_LIBRARY}")

endif()

mark_as_advanced(TestU01_INCLUDE_DIR TestU01_testu01_LIBRARY TestU01_probdist_LIBRARY
  TestU01_mylib_LIBRARY TestU01_MATH_LIBRARY)
