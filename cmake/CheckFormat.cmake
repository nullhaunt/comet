if(NOT DEFINED COMET_ROOT OR NOT DEFINED CLANG_FORMAT)
  message(FATAL_ERROR "COMET_ROOT and CLANG_FORMAT are required")
endif()
file(GLOB_RECURSE sources
  "${COMET_ROOT}/src/*.hpp"
  "${COMET_ROOT}/src/*.cpp"
  "${COMET_ROOT}/tests/*.cpp"
  "${COMET_ROOT}/tools/*.cpp"
)
execute_process(COMMAND "${CLANG_FORMAT}" --dry-run --Werror ${sources} RESULT_VARIABLE result)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Project-owned C++ files do not match .clang-format")
endif()

