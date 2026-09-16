include_guard(GLOBAL)

function(comet_apply_target_policies target)
  doggo_apply_target_policies(${target})
  target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/src")
  target_compile_definitions(${target} PRIVATE
    COMET_DEVELOPMENT_SERVICES=$<BOOL:${COMET_DEVELOPMENT_SERVICES}>
    COMET_SHIPPING=$<BOOL:${COMET_SHIPPING}>
  )
endfunction()

function(comet_add_library module)
  add_library(comet-${module} STATIC ${ARGN})
  add_library(comet::${module} ALIAS comet-${module})
  comet_apply_target_policies(comet-${module})
endfunction()

function(comet_add_executable target)
  add_executable(${target} ${ARGN})
  comet_apply_target_policies(${target})
endfunction()

function(comet_check_source_policy root)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -DCOMET_ROOT=${root} -P "${root}/cmake/CheckSourcePolicy.cmake"
    RESULT_VARIABLE result
  )
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Comet source policy check failed")
  endif()
endfunction()

