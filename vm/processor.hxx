#pragma once

#ifndef CHIP8_VM_PROCESSOR_HXX
#define CHIP8_VM_PROCESSOR_HXX

#include <cstdint>

#include "call_stack.hxx"
#include "logger.hxx"
#include "memory.hxx"
#include "screen.hxx"

namespace chip8 {
  class Processor final {
  public:
    Processor(CallStack& call_stack, Memory& memory, Screen& screen, Logger& logger) noexcept;

    Processor(Processor const&) = delete;

    Processor& operator=(Processor const&) = delete;

    bool step();

  private:
    // dependencies
    CallStack& call_stack_;
    Memory& memory_;
    Screen& screen_;
    Logger& logger_;

    // registers
    Address pc_{0x200};
    Address i_{0x0};
    std::array<std::uint8_t, 16u> v_{};

    bool native_instruction(std::uint16_t param);
    void jump(std::uint16_t param);
    void call(std::uint16_t param);
    void set_register(std::uint8_t index, std::uint8_t value);
    void add_to_register(std::uint8_t index, std::uint8_t value);
    void set_index_register(std::uint16_t value);
    void draw(std::uint8_t x_register, std::uint8_t y_register, std::uint8_t sprite_size);
  };
}

#endif // CHIP8_VM_PROCESSOR_HXX
