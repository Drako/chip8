add_library(vm STATIC
    address.hxx
    call_stack.hxx call_stack.cxx
    logger.hxx
    memory.hxx memory.cxx
    processor.hxx processor.cxx
    screen.hxx
)
target_include_directories(vm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(chip_8 WIN32
    main.cxx
)
target_link_libraries(chip_8 PRIVATE vm SDL2::SDL2 SDL2::SDL2main)

if (WIN32)
  copy_dependency_dll(TARGET chip_8 DEPENDENCY SDL2::SDL2)

  include(InstallRequiredSystemLibraries)
  install(IMPORTED_RUNTIME_ARTIFACTS SDL2::SDL2)

  install(TARGETS chip_8 DESTINATION bin)
  install(FILES ${assets} DESTINATION assets)

  set(CPACK_GENERATOR NSIS64)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "Draconic Bytes\\\\CHIP-8")
  set(CPACK_PACKAGE_NAME "CHIP-8")
  set(CPACK_PACKAGE_VENDOR "Draconic Bytes")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
  set(CPACK_PACKAGE_EXECUTABLES "chip_8;CHIP-8")
  include(CPack)
endif ()
