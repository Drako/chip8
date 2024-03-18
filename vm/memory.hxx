#pragma once

#ifndef CHIP8_VM_MEMORY_HXX
#define CHIP8_VM_MEMORY_HXX

#include <algorithm>
#include <array>
#include <cstdint>
#include <ranges>
#include <stdexcept>

#include "address.hxx"

namespace chip8 {
  /**
   * Exception class being thrown when trying put data outside of the valid memory range.
   *
   * Even though the class has the same functionality as its base,
   * it exists for improved readability as it is more specific.
   */
  class MemoryOverflowException final : public std::runtime_error {
    using std::runtime_error::runtime_error;
    using std::runtime_error::operator=;
  };

  class Memory final {
  public:
    /**
     * The default constructor.
     *
     * The memory is initially filled with zeroes.
     */
    Memory() noexcept;

    Memory(Memory const&) noexcept = default;

    Memory& operator=(Memory const&) noexcept = default;

    std::uint8_t operator[](Address address) const noexcept;

    std::uint8_t& operator[](Address address) noexcept;

    void load_default_font(Address const base) noexcept;

    /**
     * Load data from the given source.
     *
     * @tparam Source The type of the source container.
     * @param base The target starting address. The loaded data is placed there.
     * @param source The source data to be loaded.
     */
    template<std::ranges::sized_range Source>
    void load(Address const base, Source&& source)
    {
      auto const offset = static_cast<std::uint16_t>(base);
      auto const max_len = 0x1000u-offset;
      auto const len = std::ranges::size(std::forward<Source>(source));
      if (len>max_len)
        throw MemoryOverflowException{"Trying to load more data than fits in memory"};

      std::ranges::copy(std::forward<Source>(source), std::ranges::begin(memory_)+offset);
    }

  private:
    std::array<std::uint8_t, 4096u> memory_;
  };
}

#endif // CHIP8_VM_MEMORY_HXX
