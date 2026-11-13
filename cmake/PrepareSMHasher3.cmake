foreach(required UPSTREAM GENERATED FABADA_ROOT GIT_EXECUTABLE)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required} when preparing SMHasher3")
  endif()
endforeach()

execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${UPSTREAM}" rev-parse HEAD
  OUTPUT_VARIABLE revision OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE status)
i
f(NOT status EQUAL 0 OR NOT revision STREQUAL "45f870d737f54254656d8968125aa1aa796c14c1")
  message(FATAL_ERROR "SMHasher3 must be initialized at pinned revision 45f870d737f54254656d8968125aa1aa796c14c1. Run git submodule update --init --recursive.")
endif()

execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${UPSTREAM}" status --porcelain
  OUTPUT_VARIABLE dirty RESULT_VARIABLE status)

if(NOT status EQUAL 0 OR NOT dirty STREQUAL "")
  message(FATAL_ERROR "SMHasher3 submodule has local changes; restore or save them before preparing a clean integration.")
endif()

# Only this generated build directory is modified. Never patch the submodule.
file(REMOVE_RECURSE "${GENERATED}")
file(MAKE_DIRECTORY "${GENERATED}")
file(COPY "${UPSTREAM}/" DESTINATION "${GENERATED}" PATTERN ".git" EXCLUDE)

configure_file("${FABADA_ROOT}/tests/smhasher3/fabada256.cpp"
  "${GENERATED}/hashes/fabada256.cpp" COPYONLY)

execute_process(COMMAND "${GIT_EXECUTABLE}" apply --no-index
  "${FABADA_ROOT}/tests/smhasher3/integration.patch"
  WORKING_DIRECTORY "${GENERATED}" RESULT_VARIABLE status ERROR_VARIABLE error)

if(NOT status EQUAL 0)
  message(FATAL_ERROR "Could not apply Fabada's SMHasher3 integration patch: ${error}")
endif()
