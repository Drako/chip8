#include "memory.hxx"

namespace chip8 {
  Memory::Memory() noexcept
      :memory_{}
  {
  }

  std::uint8_t Memory::operator[](chip8::Address const address) const noexcept
  {
    return memory_[static_cast<std::uint16_t>(address)];
  }

  std::uint8_t& Memory::operator[](chip8::Address const address) noexcept
  {
    return memory_[static_cast<std::uint16_t>(address)];
  }

  void Memory::load_default_font(Address const base) noexcept
  {
    auto const b = static_cast<std::uint16_t>(base);
    // 0
    memory_[b+0] = 0xF0;
    memory_[b+1] = 0x90;
    memory_[b+2] = 0x90;
    memory_[b+3] = 0x90;
    memory_[b+4] = 0xF0;
    // 1
    memory_[b+5] = 0x20;
    memory_[b+6] = 0x60;
    memory_[b+7] = 0x20;
    memory_[b+8] = 0x20;
    memory_[b+9] = 0x70;
    // 2
    memory_[b+10] = 0xF0;
    memory_[b+11] = 0x10;
    memory_[b+12] = 0xF0;
    memory_[b+13] = 0x80;
    memory_[b+14] = 0xF0;
    // 3
    memory_[b+15] = 0xF0;
    memory_[b+16] = 0x10;
    memory_[b+17] = 0xF0;
    memory_[b+18] = 0x10;
    memory_[b+19] = 0xF0;
    // 4
    memory_[b+20] = 0x90;
    memory_[b+21] = 0x90;
    memory_[b+22] = 0xF0;
    memory_[b+23] = 0x10;
    memory_[b+24] = 0x10;
    // 5
    memory_[b+25] = 0xF0;
    memory_[b+26] = 0x80;
    memory_[b+27] = 0xF0;
    memory_[b+28] = 0x10;
    memory_[b+29] = 0xF0;
    // 6
    memory_[b+30] = 0xF0;
    memory_[b+31] = 0x80;
    memory_[b+32] = 0xF0;
    memory_[b+33] = 0x90;
    memory_[b+34] = 0xF0;
    // 7
    memory_[b+35] = 0xF0;
    memory_[b+36] = 0x10;
    memory_[b+37] = 0x20;
    memory_[b+38] = 0x40;
    memory_[b+39] = 0x40;
    // 8
    memory_[b+40] = 0xF0;
    memory_[b+41] = 0x90;
    memory_[b+42] = 0xF0;
    memory_[b+43] = 0x90;
    memory_[b+44] = 0xF0;
    // 9
    memory_[b+45] = 0xF0;
    memory_[b+46] = 0x90;
    memory_[b+47] = 0xF0;
    memory_[b+48] = 0x10;
    memory_[b+49] = 0xF0;
    // A
    memory_[b+50] = 0xF0;
    memory_[b+51] = 0x90;
    memory_[b+52] = 0xF0;
    memory_[b+53] = 0x90;
    memory_[b+54] = 0x90;
    // B
    memory_[b+55] = 0xE0;
    memory_[b+56] = 0x90;
    memory_[b+57] = 0xE0;
    memory_[b+58] = 0x90;
    memory_[b+59] = 0xE0;
    // C
    memory_[b+60] = 0xF0;
    memory_[b+61] = 0x80;
    memory_[b+62] = 0x80;
    memory_[b+63] = 0x80;
    memory_[b+64] = 0xF0;
    // D
    memory_[b+65] = 0xE0;
    memory_[b+66] = 0x90;
    memory_[b+67] = 0x90;
    memory_[b+68] = 0x90;
    memory_[b+69] = 0xE0;
    // E
    memory_[b+70] = 0xF0;
    memory_[b+71] = 0x80;
    memory_[b+72] = 0xF0;
    memory_[b+73] = 0x80;
    memory_[b+74] = 0xF0;
    // F
    memory_[b+75] = 0xF0;
    memory_[b+76] = 0x80;
    memory_[b+77] = 0xF0;
    memory_[b+78] = 0x80;
    memory_[b+79] = 0x80;
  }
}
