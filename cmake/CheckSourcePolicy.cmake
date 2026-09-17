if(NOT DEFINED COMET_ROOT)
  message(FATAL_ERROR "COMET_ROOT is required")
endif()

file(GLOB_RECURSE owned_files RELATIVE "${COMET_ROOT}"
  "${COMET_ROOT}/src/*"
  "${COMET_ROOT}/tests/*"
  "${COMET_ROOT}/tools/*"
)
foreach(path IN LISTS owned_files)
  if(path MATCHES "\\.(c|cc|cxx|c\\+\\+|h|hh|hxx)$")
    message(FATAL_ERROR "Forbidden C/C++ extension: ${path}; use only .cpp and .hpp")
  endif()
  if(path MATCHES "\\.hpp$" AND NOT path MATCHES "^src/")
    message(FATAL_ERROR "Every Comet header must be project-private beneath src: ${path}")
  endif()
  if(path MATCHES "^src/([a-z][a-z0-9]*)/([^/]+)\\.(cpp|hpp)$")
    set(module "${CMAKE_MATCH_1}")
    set(filename "${CMAKE_MATCH_2}")
    if(NOT filename MATCHES "^${module}_[A-Z][A-Za-z0-9]*$")
      message(FATAL_ERROR "Comet module file must use <module>_<Name>: ${path}")
    endif()
  endif()
endforeach()

file(GLOB_RECURSE scan_files "${COMET_ROOT}/src/*" "${COMET_ROOT}/assets/source/*")
foreach(path IN LISTS scan_files)
  if(NOT IS_DIRECTORY "${path}")
    file(READ "${path}" content)
    string(TOLOWER "${content}" content_lower)
    if(content_lower MATCHES "entt")
      message(FATAL_ERROR "Comet must not name EnTT in source or serialized data: ${path}")
    endif()
    if(path MATCHES "[/\\\\]src[/\\\\]" AND
       content_lower MATCHES "(^|[^a-z0-9_])(vulkan|deko3d|libnx|sdl3)([^a-z0-9_]|$)|switch[.]h")
      message(FATAL_ERROR "Comet source must use Doggo portable APIs instead of platform/backend APIs: ${path}")
    endif()
  endif()
endforeach()
