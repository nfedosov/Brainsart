#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LSL::lsl" for configuration "Debug"
set_property(TARGET LSL::lsl APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LSL::lsl PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/liblsl.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/liblsl.dll"
  )

list(APPEND _cmake_import_check_targets LSL::lsl )
list(APPEND _cmake_import_check_files_for_LSL::lsl "${_IMPORT_PREFIX}/lib/liblsl.dll.a" "${_IMPORT_PREFIX}/bin/liblsl.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
