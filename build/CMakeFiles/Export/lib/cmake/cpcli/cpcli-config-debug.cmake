#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cpcli::cpcli" for configuration "Debug"
set_property(TARGET cpcli::cpcli APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(cpcli::cpcli PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libcpcli.so"
  IMPORTED_SONAME_DEBUG "libcpcli.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS cpcli::cpcli )
list(APPEND _IMPORT_CHECK_FILES_FOR_cpcli::cpcli "${_IMPORT_PREFIX}/lib/libcpcli.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
