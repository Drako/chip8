function(copy_dependency_dll)
  set(oneValueArgs TARGET DEPENDENCY)
  cmake_parse_arguments(CDD "" "${oneValueArgs}" "" ${ARGN})

  get_target_property(full_path ${CDD_DEPENDENCY} IMPORTED_LOCATION)
  get_filename_component(lib_name "${full_path}" NAME)

  add_custom_command(
      TARGET ${CDD_TARGET}
      POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy "${full_path}" "${CMAKE_BINARY_DIR}/${lib_name}"
      COMMENT "Copying ${CDD_DEPENDENCY}..."
  )
endfunction()

function(get_dependency_location result_var)
  set(oneValueArgs DEPENDENCY)
  cmake_parse_arguments(GDL "" "${oneValueArgs}" "" ${ARGN})

  get_target_property(full_path ${GDL_DEPENDENCY} IMPORTED_LOCATION)
  get_filename_component(${result_var} "${full_path}" DIRECTORY)
  return(PROPAGATE ${result_var})
endfunction()
