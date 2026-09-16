if(NOT DEFINED COMET_ROOT OR NOT DEFINED CLANG_TIDY OR NOT DEFINED TEST_BINARY_DIR)
  message(FATAL_ERROR "COMET_ROOT, CLANG_TIDY, and TEST_BINARY_DIR are required")
endif()

file(MAKE_DIRECTORY "${TEST_BINARY_DIR}")
set(valid_source "${TEST_BINARY_DIR}/clang_tidy_naming_valid.cpp")
set(invalid_source "${TEST_BINARY_DIR}/clang_tidy_naming_invalid.cpp")
configure_file("${COMET_ROOT}/tests/policy/clang_tidy_naming_valid.cpp.in" "${valid_source}" COPYONLY)
configure_file("${COMET_ROOT}/tests/policy/clang_tidy_naming_invalid.cpp.in" "${invalid_source}" COPYONLY)

execute_process(
  COMMAND "${CLANG_TIDY}" --verify-config "--config-file=${COMET_ROOT}/.clang-tidy"
  RESULT_VARIABLE verify_result
  OUTPUT_VARIABLE verify_output
  ERROR_VARIABLE verify_error
)
if(NOT verify_result EQUAL 0)
  message(FATAL_ERROR ".clang-tidy contains an invalid check or option:\n${verify_output}${verify_error}")
endif()

set(naming_options
  "--config-file=${COMET_ROOT}/.clang-tidy"
  "--checks=-*,readability-identifier-naming"
  "--warnings-as-errors=readability-identifier-naming"
)
execute_process(
  COMMAND "${CLANG_TIDY}" "${valid_source}" ${naming_options} -- -std=c++20
  RESULT_VARIABLE valid_result
  OUTPUT_VARIABLE valid_output
  ERROR_VARIABLE valid_error
)
if(NOT valid_result EQUAL 0)
  message(FATAL_ERROR "Valid naming fixture was rejected:\n${valid_output}${valid_error}")
endif()

execute_process(
  COMMAND "${CLANG_TIDY}" "${invalid_source}" ${naming_options} -- -std=c++20
  RESULT_VARIABLE invalid_result
  OUTPUT_VARIABLE invalid_output
  ERROR_VARIABLE invalid_error
)
set(invalid_log "${invalid_output}${invalid_error}")
if(invalid_result EQUAL 0)
  message(FATAL_ERROR "Invalid naming fixture was accepted")
endif()

foreach(identifier IN ITEMS
    invalid_global
    invalid_global_constant
    invalid_member
    invalid_member_constant
    invalid_static_member
    invalid_static_member_constant
    invalid_method
    invalid_parameter
    invalid_local
    invalid_local_constant
    invalid_static_local
    invalid_static_local_constant
    invalid_function)
  string(FIND "${invalid_log}" "'${identifier}'" diagnostic_index)
  if(diagnostic_index EQUAL -1)
    message(FATAL_ERROR "Missing naming diagnostic for '${identifier}':\n${invalid_log}")
  endif()
endforeach()
