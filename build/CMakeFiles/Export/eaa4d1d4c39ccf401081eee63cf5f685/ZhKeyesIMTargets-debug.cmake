#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ZhKeyesIM::util" for configuration "Debug"
set_property(TARGET ZhKeyesIM::util APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ZhKeyesIM::util PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libutild.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/utild.dll"
  )

list(APPEND _cmake_import_check_targets ZhKeyesIM::util )
list(APPEND _cmake_import_check_files_for_ZhKeyesIM::util "${_IMPORT_PREFIX}/lib/libutild.dll.a" "${_IMPORT_PREFIX}/bin/utild.dll" )

# Import target "ZhKeyesIM::net" for configuration "Debug"
set_property(TARGET ZhKeyesIM::net APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ZhKeyesIM::net PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libnetd.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/netd.dll"
  )

list(APPEND _cmake_import_check_targets ZhKeyesIM::net )
list(APPEND _cmake_import_check_files_for_ZhKeyesIM::net "${_IMPORT_PREFIX}/lib/libnetd.dll.a" "${_IMPORT_PREFIX}/bin/netd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
