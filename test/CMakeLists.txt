add_executable(chip8_tests
    address_test.cxx
    call_stack_test.cxx
    memory_test.cxx
    processor_test.cxx
)
target_link_libraries(chip8_tests PRIVATE Catch2::Catch2WithMain vm)

add_test(NAME Tests COMMAND chip8_tests)
