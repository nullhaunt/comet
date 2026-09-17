cmake_minimum_required(VERSION 3.25)

project(Comet VERSION 0.1.0 LANGUAGES C CXX)

set(COMET_PLATFORM "windows" CACHE STRING "Comet platform implementation")
set_property(CACHE COMET_PLATFORM PROPERTY STRINGS windows switch)
option(COMET_BUILD_TESTS "Build Comet tests" ON)
option(COMET_DEVELOPMENT_SERVICES "Include development-only facilities" ON)
option(COMET_SHIPPING "Enforce the shipping target graph" OFF)
set(COMET_HOST_TOOLS_DIR "" CACHE PATH "Absolute directory containing tools built for the host")

if(NOT COMET_PLATFORM STREQUAL "windows" AND NOT COMET_PLATFORM STREQUAL "switch")
  message(FATAL_ERROR "COMET_PLATFORM must be 'windows' or 'switch', got '${COMET_PLATFORM}'")
endif()
if(COMET_SHIPPING AND COMET_DEVELOPMENT_SERVICES)
  message(FATAL_ERROR "Shipping builds cannot enable COMET_DEVELOPMENT_SERVICES")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/doggo/CMakeLists.txt")
  message(FATAL_ERROR "Doggo submodule is missing. Run: git submodule update --init --recursive lib/doggo")
endif()
if(COMET_PLATFORM STREQUAL "switch")
  if(NOT IS_ABSOLUTE "${COMET_HOST_TOOLS_DIR}")
    message(FATAL_ERROR "Switch configuration requires absolute COMET_HOST_TOOLS_DIR from a completed host build")
  endif()
  if(NOT EXISTS "${COMET_HOST_TOOLS_DIR}/comet-cooker.exe")
    message(FATAL_ERROR "Expected host cooker at '${COMET_HOST_TOOLS_DIR}/comet-cooker.exe'; build the windows-clang-dev comet-cooker target first")
  endif()
  add_executable(comet-cooker-host IMPORTED GLOBAL)
  set_target_properties(comet-cooker-host PROPERTIES IMPORTED_LOCATION "${COMET_HOST_TOOLS_DIR}/comet-cooker.exe")
endif()

set(DOGGO_PLATFORM "${COMET_PLATFORM}" CACHE STRING "Doggo platform implementation" FORCE)
set(DOGGO_DEVELOPMENT_SERVICES "${COMET_DEVELOPMENT_SERVICES}" CACHE BOOL "Doggo development facilities" FORCE)
set(DOGGO_SHIPPING "${COMET_SHIPPING}" CACHE BOOL "Doggo shipping graph" FORCE)
add_subdirectory(lib/doggo)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CometPolicies)
comet_check_source_policy("${CMAKE_CURRENT_SOURCE_DIR}")

if(COMET_PLATFORM STREQUAL "windows")
  find_package(Vulkan 1.4.357 REQUIRED COMPONENTS glslc)
  set(COMET_SHADER_COMPILER "$<TARGET_FILE:Vulkan::glslc>")
  set(COMET_SHADER_FORMAT spirv)
  set(COMET_FIXTURE_SHADER_EXTENSION spv)
  set(COMET_FIXTURE_SHADER_IS_DKSH 0)
  set(COMET_DIAGNOSTIC_LOG_PATH "")
else()
  find_program(COMET_UAM_EXECUTABLE NAMES uam HINTS "$ENV{DEVKITPRO}/tools/bin" REQUIRED)
  set(COMET_SHADER_COMPILER "${COMET_UAM_EXECUTABLE}")
  set(COMET_SHADER_FORMAT dksh)
  set(COMET_FIXTURE_SHADER_EXTENSION dksh)
  set(COMET_FIXTURE_SHADER_IS_DKSH 1)
  set(COMET_DIAGNOSTIC_LOG_PATH "sdmc:/comet-gate-2.log")
endif()

doggo_add_shader_manifest(comet-fixture-shaders
  MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/assets/source/shaders/fixtures/textured.shader.json"
  FORMAT "${COMET_SHADER_FORMAT}"
  COMPILER "${COMET_SHADER_COMPILER}"
)
get_target_property(COMET_FIXTURE_SHADER_DIRECTORY comet-fixture-shaders DOGGO_SHADER_OUTPUT_DIRECTORY)
get_target_property(COMET_FIXTURE_SHADER_OUTPUTS comet-fixture-shaders DOGGO_SHADER_OUTPUTS)

if(COMET_PLATFORM STREQUAL "switch")
  set(COMET_FIXTURE_ROMFS_ROOT "${CMAKE_CURRENT_BINARY_DIR}/generated/romfs/comet-app")
  set(COMET_FIXTURE_ROMFS_SHADER_DIRECTORY
    "${COMET_FIXTURE_ROMFS_ROOT}/shaders/comet.fixture.textured"
  )
  set(COMET_FIXTURE_ROMFS_OUTPUTS
    "${COMET_FIXTURE_ROMFS_SHADER_DIRECTORY}/vertex.dksh"
    "${COMET_FIXTURE_ROMFS_SHADER_DIRECTORY}/fragment.dksh"
    "${COMET_FIXTURE_ROMFS_SHADER_DIRECTORY}/comet.fixture.textured.shader.json"
  )
  add_custom_command(
    OUTPUT ${COMET_FIXTURE_ROMFS_OUTPUTS}
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${COMET_FIXTURE_ROMFS_SHADER_DIRECTORY}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${COMET_FIXTURE_SHADER_DIRECTORY}/vertex.dksh"
      "${COMET_FIXTURE_ROMFS_SHADER_DIRECTORY}/vertex.dksh"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${COMET_FIXTURE_SHADER_DIRECTORY}/fragment.dksh"
      "${COMET_FIXTURE_ROMFS_SHADER_DIRECTORY}/fragment.dksh"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${COMET_FIXTURE_SHADER_DIRECTORY}/comet.fixture.textured.shader.json"
      "${COMET_FIXTURE_ROMFS_SHADER_DIRECTORY}/comet.fixture.textured.shader.json"
    DEPENDS ${COMET_FIXTURE_SHADER_OUTPUTS}
    COMMENT "Staging comet.fixture.textured in Switch ROMFS"
    VERBATIM
  )
  set(COMET_FIXTURE_VERTEX_SHADER_PATH
    "romfs:/shaders/comet.fixture.textured/vertex.${COMET_FIXTURE_SHADER_EXTENSION}"
  )
  set(COMET_FIXTURE_FRAGMENT_SHADER_PATH
    "romfs:/shaders/comet.fixture.textured/fragment.${COMET_FIXTURE_SHADER_EXTENSION}"
  )
else()
  set(COMET_FIXTURE_VERTEX_SHADER_PATH
    "${COMET_FIXTURE_SHADER_DIRECTORY}/vertex.${COMET_FIXTURE_SHADER_EXTENSION}"
  )
  set(COMET_FIXTURE_FRAGMENT_SHADER_PATH
    "${COMET_FIXTURE_SHADER_DIRECTORY}/fragment.${COMET_FIXTURE_SHADER_EXTENSION}"
  )
endif()

comet_add_library(components
  src/components/components_BootState.cpp
  src/components/components_BootState.hpp
  src/components/components_Registration.cpp
  src/components/components_Registration.hpp
)
target_link_libraries(comet-components PUBLIC doggo::core)

comet_add_library(gameplay
  src/gameplay/gameplay_Systems.cpp
  src/gameplay/gameplay_Systems.hpp
)
target_link_libraries(comet-gameplay PUBLIC comet::components doggo::core)

comet_add_library(presentation
  src/presentation/presentation_RenderFixture.cpp
  src/presentation/presentation_RenderFixture.hpp
  src/presentation/presentation_State.cpp
  src/presentation/presentation_State.hpp
)
target_compile_definitions(comet-presentation PRIVATE
  COMET_FIXTURE_SHADER_IS_DKSH=${COMET_FIXTURE_SHADER_IS_DKSH}
  "COMET_FIXTURE_VERTEX_SHADER_PATH=\"${COMET_FIXTURE_VERTEX_SHADER_PATH}\""
  "COMET_FIXTURE_FRAGMENT_SHADER_PATH=\"${COMET_FIXTURE_FRAGMENT_SHADER_PATH}\""
  "COMET_DIAGNOSTIC_LOG_PATH=\"${COMET_DIAGNOSTIC_LOG_PATH}\""
)
target_link_libraries(comet-presentation PUBLIC
  comet::components
  doggo::core
  doggo::math
  doggo::platform
  doggo::rhi
)

comet_add_executable(comet-app src/Main.cpp)
target_link_libraries(comet-app PRIVATE
  comet::components
  comet::gameplay
  comet::presentation
  doggo::core
  doggo::platform
)
add_dependencies(comet-app comet-fixture-shaders)
if(COMET_PLATFORM STREQUAL "switch")
  doggo_add_switch_package(comet-app "Comet Gate 2 Fixture" "Comet"
    ROMFS "${COMET_FIXTURE_ROMFS_ROOT}"
    DEPENDS ${COMET_FIXTURE_ROMFS_OUTPUTS}
  )
endif()

if(NOT COMET_PLATFORM STREQUAL "switch" AND NOT COMET_SHIPPING)
  comet_add_executable(comet-cooker tools/cooker/Main.cpp)
  target_link_libraries(comet-cooker PRIVATE comet::components doggo::core)

  if(COMET_DEVELOPMENT_SERVICES)
    comet_add_executable(comet-editor tools/editor/Main.cpp)
    target_link_libraries(comet-editor PRIVATE comet::components doggo::core)
  endif()
endif()

if(COMET_BUILD_TESTS AND NOT COMET_PLATFORM STREQUAL "switch")
  include(CTest)
  enable_testing()

  comet_add_executable(comet-composition-tests tests/app/app_Composition.cpp)
  target_link_libraries(comet-composition-tests PRIVATE
    comet::components
    comet::gameplay
    comet::presentation
    doggo::core
  )
  add_test(NAME comet-composition COMMAND comet-composition-tests)

  add_test(
    NAME comet-source-policy
    COMMAND "${CMAKE_COMMAND}" -DCOMET_ROOT=${CMAKE_CURRENT_SOURCE_DIR} -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckSourcePolicy.cmake"
  )

  find_program(COMET_CLANG_FORMAT_EXECUTABLE NAMES clang-format)
  if(COMET_CLANG_FORMAT_EXECUTABLE)
    add_test(
      NAME comet-format
      COMMAND "${CMAKE_COMMAND}" -DCOMET_ROOT=${CMAKE_CURRENT_SOURCE_DIR} -DCLANG_FORMAT=${COMET_CLANG_FORMAT_EXECUTABLE} -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckFormat.cmake"
    )
    add_custom_target(comet-format-check
      COMMAND "${CMAKE_COMMAND}" -DCOMET_ROOT=${CMAKE_CURRENT_SOURCE_DIR} -DCLANG_FORMAT=${COMET_CLANG_FORMAT_EXECUTABLE} -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckFormat.cmake"
      VERBATIM
    )
  else()
    message(STATUS "clang-format not found; comet-format test is unavailable")
  endif()

  get_filename_component(COMET_CXX_COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
  find_program(COMET_CLANG_TIDY_EXECUTABLE NAMES clang-tidy HINTS "${COMET_CXX_COMPILER_DIR}")
  if(COMET_CLANG_TIDY_EXECUTABLE)
    set(COMET_CLANG_TIDY_TEST_COMMAND
      "${CMAKE_COMMAND}"
      "-DCOMET_ROOT=${CMAKE_CURRENT_SOURCE_DIR}"
      "-DCLANG_TIDY=${COMET_CLANG_TIDY_EXECUTABLE}"
      "-DTEST_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/clang-tidy-naming"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckClangTidyNaming.cmake"
    )
    add_test(NAME comet-clang-tidy-naming COMMAND ${COMET_CLANG_TIDY_TEST_COMMAND})
    add_custom_target(comet-clang-tidy-naming-check COMMAND ${COMET_CLANG_TIDY_TEST_COMMAND} VERBATIM)
  else()
    message(STATUS "clang-tidy not found; comet-clang-tidy-naming test is unavailable")
  endif()
endif()
