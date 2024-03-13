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
}
