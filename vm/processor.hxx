#pragma once

#ifndef CHIP8_VM_PROCESSOR_HXX
#define CHIP8_VM_PROCESSOR_HXX

#include <atomic>
#include <cstdint>

#include "call_stack.hxx"
#include "logger.hxx"
#include "memory.hxx"
#include "screen.hxx"

namespace chip8 {
  struct Config final {
    bool register_rw_modifies_i;
  };

  class Processor final {
  public:
    static constexpr Address const CODE_START = 0x200_addr;
    static constexpr Address const FONT_START = 0x050_addr;

    Processor(Config const& config, CallStack& call_stack, Memory& memory, Screen& screen, Logger& logger) noexcept;

    Processor(Processor const&) = delete;

    Processor& operator=(Processor const&) = delete;

    bool step();

    void update_timers();

    void toggle_key(std::uint8_t index, bool pressed);

  private:
    // dependencies
    Config config_;
    CallStack& call_stack_;
    Memory& memory_;
    Screen& screen_;
    Logger& logger_;

    // registers
    Address pc_{0x200};
    Address i_{0x0};
    std::atomic<std::uint8_t> delay_timer_{0u};
    // std::atomic<std::uint8_t> sound_timer_{0u};
    std::atomic<std::uint16_t> keys_{0u};
    std::array<std::uint8_t, 16u> v_{};

    bool native_instruction(std::uint16_t param);

    void jump(std::uint16_t param);

    void call(std::uint16_t param);

    void set_register(std::uint8_t index, std::uint8_t value);

    void add_to_register(std::uint8_t index, std::uint8_t value);

    void set_index_register(std::uint16_t value);

    void add_to_index_register(std::uint8_t index);

    void draw(std::uint8_t x_register, std::uint8_t y_register, std::uint8_t sprite_size);

    bool register_instruction(std::uint8_t index, std::uint16_t instruction);

    void store_to_memory(std::uint8_t index);

    void load_from_memory(std::uint8_t index);

    void get_delay_timer(std::uint8_t index);

    void set_delay_timer(std::uint8_t index);

    void skip_if_equal_to(std::uint8_t index, std::uint8_t value);

    void skip_unless_equal_to(std::uint8_t index, std::uint8_t value);

    void skip_if_equal(std::uint8_t x, std::uint8_t y);

    void skip_unless_equal(std::uint8_t x, std::uint8_t y);

    bool key_skips(std::uint8_t index, std::uint8_t instruction);

    void skip_if_pressed(std::uint8_t index);

    void skip_unless_pressed(std::uint8_t index);

    void get_key(std::uint8_t index);

    void font_character(std::uint8_t const index);
  };
}

#endif // CHIP8_VM_PROCESSOR_HXX
