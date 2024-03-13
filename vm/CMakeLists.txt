add_library(vm STATIC
    address.hxx
    call_stack.hxx call_stack.cxx
    memory.hxx memory.cxx
)
target_include_directories(vm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(chip_8 WIN32
    main.cxx
)
target_link_libraries(chip_8 PRIVATE vm SDL2::SDL2 SDL2::SDL2main)
