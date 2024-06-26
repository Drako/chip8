#pragma once

#ifndef CHIP8_VM_PROCESSOR_HXX
#define CHIP8_VM_PROCESSOR_HXX

#include <atomic>
#include <cstdint>
#include <random>

#include "call_stack.hxx"
#include "logger.hxx"
#include "memory.hxx"
#include "screen.hxx"

namespace chip8 {
  struct Config final {
    bool register_rw_modifies_i;
    bool shift_takes_value_from_vy;
    bool use_vx_for_offset_jump;
  };

  enum class GetKeyState {
    None,
    WaitingForKey,
    GotKey,
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

    std::mt19937 rng_{std::random_device{}()};
    std::uniform_int_distribution<std::uint16_t> dist_{0, 0xFF};

    // registers
    Address pc_{0x200};
    Address i_{0x0};
    std::atomic<std::uint8_t> delay_timer_{0u};
    // std::atomic<std::uint8_t> sound_timer_{0u};
    std::array<std::uint8_t, 16u> v_{};

    std::atomic<std::uint16_t> keys_{0u};
    GetKeyState get_key_state_ = GetKeyState::None;
    std::uint8_t last_key_{0};

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

    void font_character(std::uint8_t index);

    bool binary_operator(std::uint8_t x, std::uint8_t y, std::uint8_t instruction);

    void assign_y_to_x(std::uint8_t x, std::uint8_t y);

    void binary_or(std::uint8_t x, std::uint8_t y);

    void binary_and(std::uint8_t x, std::uint8_t y);

    void binary_xor(std::uint8_t x, std::uint8_t y);

    void add_y_to_x(std::uint8_t x, std::uint8_t y);

    void subtract_y_from_x(std::uint8_t x, std::uint8_t y);

    void subtract_x_from_y(std::uint8_t x, std::uint8_t y);

    void shift_right(std::uint8_t x, std::uint8_t y);

    void shift_left(std::uint8_t x, std::uint8_t y);

    void binary_coded_decimal(std::uint8_t index);

    void random_number(std::uint8_t x, std::uint8_t mask);

    void jump_with_offset(std::uint8_t const x, std::uint16_t const nnn);
  };
}

#endif // CHIP8_VM_PROCESSOR_HXX
