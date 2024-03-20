#include "processor.hxx"

#include <iomanip>
#include <sstream>

namespace chip8 {
  Processor::Processor(Config const& config, CallStack& call_stack, Memory& memory, Screen& screen,
      Logger& logger) noexcept
      :config_{config}, call_stack_{call_stack}, memory_{memory}, screen_{screen}, logger_{logger}
  {
    memory_.load_default_font(FONT_START);
  }

  bool Processor::step()
  {
    auto const first_byte = memory_[pc_++];
    auto const nn = memory_[pc_++];

    auto const first_nibble = static_cast<std::uint8_t>(first_byte >> 4);
    auto const x = static_cast<std::uint8_t>(first_byte & 0xFu);
    auto const y = static_cast<std::uint8_t>(nn>4);
    auto const n = static_cast<std::uint8_t>(nn & 0xFu);

    auto const nnn = static_cast<std::uint16_t>((x << 8u) | nn);

    switch (first_nibble) {
    default: {
      std::ostringstream msg;
      msg << "Unsupported instruction 0x"
          << std::setfill('0') << std::setw(4) << std::hex
          << ((first_byte << 8) | nn)
          << " at 0x"
          << std::setfill('0') << std::setw(3) << std::hex
          << static_cast<std::uint16_t>(pc_)-2;
      logger_.error(msg.str().c_str());
    }
      return false;
    case 0x0:
      return native_instruction(nnn);
    case 0x1:
      jump(nnn);
      return true;
    case 0x2:
      call(nnn);
      return true;
    case 0x3:
      skip_if_equal_to(x, nn);
      return true;
    case 0x4:
      skip_unless_equal_to(x, nn);
      return true;
    case 0x5:
      skip_if_equal(x, y);
      return true;
    case 0x6:
      set_register(x, nn);
      return true;
    case 0x7:
      add_to_register(x, nn);
      return true;
    case 0x9:
      skip_unless_equal(x, y);
      return true;
    case 0xA:
      set_index_register(nnn);
      return true;
    case 0xD:
      draw(x, y, n);
      return true;
    case 0xE:
      return key_skips(x, nn);
    case 0xF:
      return register_instruction(x, nn);
    }
  }

  bool Processor::native_instruction(std::uint16_t const param)
  {
    switch (param) {
    default: {
      std::ostringstream msg;
      msg << "Unsupported native instruction 0x"
          << std::setfill('0') << std::setw(3) << std::hex
          << (param & Address::VALUE_MASK)
          << " at 0x"
          << std::setfill('0') << std::setw(3) << std::hex
          << static_cast<std::uint16_t>(pc_)-2;
      logger_.error(msg.str().c_str());
    }
      return false;
    case 0x0E0:
      logger_.debug("Instruction: Clear screen");
      screen_.clear();
      return true;
    case 0x0EE:
      logger_.debug("Instruction: Return");
      if (auto const next_pc = call_stack_.pop(); next_pc.has_value()) {
        std::ostringstream msg;
        msg << "Returning to 0x"
            << std::setfill('0') << std::setw(3) << std::hex
            << static_cast<std::uint16_t>(*next_pc);
        logger_.debug(msg.str().c_str());
        pc_ = *next_pc;
        return true;
      }
      else {
        logger_.error("No return address on stack");
        return false;
      }
    }
  }

  void Processor::jump(std::uint16_t const param)
  {
    logger_.debug("Instruction: Jump");
    std::ostringstream msg;
    msg << "Jumping to 0x"
        << std::setfill('0') << std::setw(3) << std::hex << (param & Address::VALUE_MASK);
    logger_.debug(msg.str().c_str());
    pc_ = Address{param, Address::Truncate{}};
  }

  void Processor::call(std::uint16_t param)
  {
    logger_.debug("Instruction: Call");
    std::ostringstream msg;
    msg << "Jumping to 0x"
        << std::setfill('0') << std::setw(3) << std::hex << (param & Address::VALUE_MASK);
    logger_.debug(msg.str().c_str());
    call_stack_.push(pc_);
    pc_ = Address{param, Address::Truncate{}};
  }

  void Processor::set_register(std::uint8_t const index, std::uint8_t const value)
  {
    logger_.debug("Instruction: Set register");
    std::ostringstream msg;
    msg << "V"
        << std::hex << static_cast<int>(index)
        << " = " << static_cast<int>(value);
    logger_.debug(msg.str().c_str());
    v_[index] = value;
  }

  void Processor::add_to_register(std::uint8_t const index, std::uint8_t const value)
  {
    logger_.debug("Instruction: Add value to register");
    std::ostringstream msg;
    msg << "V"
        << std::hex << static_cast<int>(index)
        << " += " << static_cast<int>(value);
    logger_.debug(msg.str().c_str());
    v_[index] += value;
  }

  void Processor::set_index_register(std::uint16_t const value)
  {
    logger_.debug("Instruction: Set index register");
    std::ostringstream msg;
    msg << "I = " << value;
    logger_.debug(msg.str().c_str());
    i_ = Address{value, Address::Truncate{}};
  }

  void Processor::draw(std::uint8_t const x_register, std::uint8_t const y_register, std::uint8_t const sprite_size)
  {
    logger_.debug("Instruction: Draw");

    auto const start_x = static_cast<std::uint8_t>(v_[x_register]%Screen::WIDTH);
    auto const start_y = static_cast<std::uint8_t>(v_[y_register]%Screen::HEIGHT);
    v_[0xF] = 0;

    auto x = start_x, y = start_y;
    for (int n = 0; n<sprite_size; ++n) {
      auto byte = memory_[i_+n];
      for (int bit = 8; bit--; byte <<= 1) {
        bool const pixel = !!(byte & 0x80);
        if (pixel) {
          if (screen_.get_pixel(x, y)) {
            v_[0xF] = 1;
            screen_.set_pixel(x, y, false);
          }
          else
            screen_.set_pixel(x, y, true);
        }

        if (++x==Screen::WIDTH) {
          break;
        }
      }
      x = start_x;
      if (++y==Screen::HEIGHT)
        break;
    }
  }

  bool Processor::register_instruction(std::uint8_t const index, std::uint16_t const instruction)
  {
    switch (instruction) {
    default: {
      std::ostringstream msg;
      msg << "Unsupported register instruction 0x"
          << std::setfill('0') << std::setw(2) << std::hex
          << instruction
          << " at 0x"
          << std::setfill('0') << std::setw(3) << std::hex
          << static_cast<std::uint16_t>(pc_)-2;
      logger_.error(msg.str().c_str());
    }
      return false;
    case 0x07:
      get_delay_timer(index);
      return true;
    case 0x0A:
      get_key(index);
      return true;
    case 0x15:
      set_delay_timer(index);
      return true;
    case 0x18:
      logger_.warn("Ignoring sound timer");
      return true;
    case 0x1E:
      add_to_index_register(index);
      return true;
    case 0x29:
      font_character(index);
      return true;
    case 0x55:
      store_to_memory(index);
      return true;
    case 0x65:
      load_from_memory(index);
      return true;
    }
  }

  void Processor::store_to_memory(std::uint8_t const index)
  {
    logger_.debug("Instruction: Store registers to memory");
    for (std::uint8_t n = 0; n<=index; ++n) {
      memory_[i_+n] = v_[n];
    }
    if (config_.register_rw_modifies_i)
      i_ += index+1;
  }

  void Processor::load_from_memory(std::uint8_t const index)
  {
    logger_.debug("Instruction: Load registers from memory");
    for (std::uint8_t n = 0; n<=index; ++n) {
      v_[n] = memory_[i_+n];
    }
    if (config_.register_rw_modifies_i)
      i_ += index+1;
  }

  void Processor::update_timers()
  {
    if (delay_timer_>0u)
      --delay_timer_;

    // if (sound_timer_>0u)
    //   --sound_timer_;
  }

  void Processor::get_delay_timer(std::uint8_t const index)
  {
    logger_.debug("Instruction: Get delay timer");
    v_[index] = delay_timer_;
  }

  void Processor::set_delay_timer(std::uint8_t const index)
  {
    logger_.debug("Instruction: Set delay timer");
    delay_timer_ = v_[index];
  }

  void Processor::skip_if_equal_to(std::uint8_t const index, std::uint8_t const value)
  {
    logger_.debug("Instruction: Skip if equal to constant");
    if (v_[index]==value)
      pc_ += 2;
  }

  void Processor::skip_unless_equal_to(std::uint8_t const index, std::uint8_t const value)
  {
    logger_.debug("Instruction: Skip unless equal to constant");
    if (v_[index]!=value)
      pc_ += 2;
  }

  void Processor::skip_if_equal(std::uint8_t const x, std::uint8_t const y)
  {
    logger_.debug("Instruction: Skip if registers are equal");
    if (v_[x]==v_[y])
      pc_ += 2;
  }

  void Processor::skip_unless_equal(std::uint8_t const x, std::uint8_t const y)
  {
    logger_.debug("Instruction: Skip unless registers are equal");
    if (v_[x]!=v_[y])
      pc_ += 2;
  }

  void Processor::toggle_key(std::uint8_t const index, bool const pressed)
  {
    if (index>0xF)
      return;

    if (pressed)
      keys_ |= (1u << index);
    else
      keys_ &= ~(1u << index);
  }

  bool Processor::key_skips(std::uint8_t const index, std::uint8_t const instruction)
  {
    switch (instruction) {
    default: {
      std::ostringstream msg;
      msg << "Unsupported key skip 0x"
          << std::setfill('0') << std::setw(2) << std::hex
          << instruction
          << " at 0x"
          << std::setfill('0') << std::setw(3) << std::hex
          << static_cast<std::uint16_t>(pc_)-2;
      logger_.error(msg.str().c_str());
    }
      return false;
    case 0x9E:
      skip_if_pressed(index);
      return true;
    case 0xA1:
      skip_unless_pressed(index);
      return true;
    }
  }

  void Processor::skip_if_pressed(std::uint8_t const index)
  {
    logger_.debug("Instruction: Skip if key pressed");
    if (keys_ & (1u << v_[index]))
      pc_ += 2;
  }

  void Processor::skip_unless_pressed(std::uint8_t const index)
  {
    logger_.debug("Instruction: Skip unless key pressed");
    if (!(keys_ & (1u << v_[index])))
      pc_ += 2;
  }

  void Processor::get_key(std::uint8_t const index)
  {
    logger_.debug("Instruction: Get key");
    std::uint16_t const keys = keys_;
    if (keys==0u) {
      pc_ += -2;
      return;
    }

    std::uint8_t key = 0;
    for (std::uint8_t i = 0; i<16; ++i) {
      if (keys & (1u << i)) {
        v_[index] = i;
        break;
      }
    }
    v_[index] = key;
  }

  void Processor::add_to_index_register(std::uint8_t const index)
  {
    logger_.debug("Instruction: Add to index register");
    i_ += v_[index];
  }

  void Processor::font_character(std::uint8_t const index)
  {
    logger_.debug("Instruction: Font character");
    int const c = (v_[index] & 0xF);

    std::ostringstream msg;
    msg << "Loading character for " << std::hex << c;
    logger_.debug(msg.str().c_str());

    i_ = FONT_START+c;
  }
}
