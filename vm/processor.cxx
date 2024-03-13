#include "processor.hxx"

namespace chip8 {
  Processor::Processor(CallStack& call_stack, Memory& memory, Screen& screen) noexcept
      :call_stack_{call_stack}, memory_{memory}, screen_{screen}
  {
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
    default:
      return false;
    case 0x0:
      return native_instruction(nnn);
    case 0x1:
      jump(nnn);
      return true;
    case 0x2:
      call(nnn);
      return true;
    case 0x6:
      set_register(x, nn);
      return true;
    case 0x7:
      add_to_register(x, nn);
      return true;
    case 0xA:
      set_index_register(nnn);
      return true;
    case 0xD:
      draw(x, y, n);
      return true;
    }
  }

  bool Processor::native_instruction(std::uint16_t const param)
  {
    switch (param) {
    default:
      return false;
    case 0x0E0:
      screen_.clear();
      return true;
    case 0x0EE:
      if (auto const next_pc = call_stack_.pop(); next_pc.has_value()) {
        pc_ = *next_pc;
        return true;
      }
      else
        return false;
    }
  }

  void Processor::jump(std::uint16_t const param)
  {
    pc_ = Address{param, Address::Truncate{}};
  }

  void Processor::call(std::uint16_t param)
  {
    call_stack_.push(pc_);
    pc_ = Address{param, Address::Truncate{}};
  }

  void Processor::set_register(std::uint8_t const index, std::uint8_t const value)
  {
    v_[index] = value;
  }

  void Processor::add_to_register(std::uint8_t const index, std::uint8_t const value)
  {
    v_[index] += value;
  }

  void Processor::set_index_register(std::uint16_t const value)
  {
    i_ = Address{value, Address::Truncate{}};
  }

  void Processor::draw(std::uint8_t const x_register, std::uint8_t const y_register, std::uint8_t const sprite_size)
  {
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
          x = start_x;
          break;
        }
      }
      if (++y==Screen::HEIGHT)
        break;
    }
  }
}
