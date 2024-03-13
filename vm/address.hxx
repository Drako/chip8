#pragma once

#ifndef CHIP8_VM_ADDRESS_HXX
#define CHIP8_VM_ADDRESS_HXX

#include <compare>
#include <cstdint>
#include <stdexcept>

namespace chip8 {
  /**
   * Exception class being thrown when trying to construct an Address from
   * a value that does not fit in 12 bits.
   *
   * Even though the class has the same functionality as its base,
   * it exists for improved readability as it is more specific.
   */
  class InvalidAddressException final : public std::domain_error {
    using std::domain_error::domain_error;
    using std::domain_error::operator=;
  };

  /**
   * Class representing an address in CHIP-8's memory.
   *
   * CHIP-8 only supports 12 bit addresses, which is fine, as the memory is also fixed to 4kiB,
   * which is fully addressable by 12 bits.
   */
  class Address final {
  public:
    static std::uint16_t constexpr VALUE_MASK = 0x0FFF;

    /**
     * Marker class to construct an address from arbitrary values by truncating them to 12 bits.
     */
    struct Truncate final {
    };

    /**
     * Construct an address from a raw value.
     * @param value The raw address. Must fit in 12 bits.
     * @throws InvalidAddressException if the address is too big.
     */
    constexpr explicit Address(std::uint16_t const value = 0U)
        :value_{value}
    {
      if ((value_ & (~VALUE_MASK))!=0U) {
        // this exception is used to force a compilation error when constructing an Address
        // using the user-defined literal with a value that does not fit in 12 bits
        throw InvalidAddressException{"Provided address does not fit in 12 bits."};
      }
    }

    /**
     * Construct an address from a raw value by truncating it to 12 bits.
     * @param value The raw address. Will be truncated to 12 bits.
     */
    constexpr Address(std::uint16_t const value, Truncate const) noexcept
        :value_{static_cast<std::uint16_t>(value & VALUE_MASK)}
    {
    }

    constexpr Address(Address const&) noexcept = default;

    ~Address() noexcept = default;

    constexpr Address& operator=(Address const&) noexcept = default;

    constexpr std::strong_ordering operator<=>(Address const& rhs) const noexcept = default;

    constexpr Address operator+(int const offset) const noexcept
    {
      return Address{static_cast<std::uint16_t>(value_+offset), Address::Truncate{}};
    }

    /**
     * Overload of prefix increment operator.
     */
    constexpr Address& operator++() noexcept
    {
      value_ = (value_+1) & VALUE_MASK;
      return *this;
    }

    /**
     * Overload of prefix decrement operator.
     */
    constexpr Address& operator--() noexcept
    {
      value_ = (value_-1) & VALUE_MASK;
      return *this;
    }

    /**
     * Overload of postfix increment operator.
     */
    constexpr Address operator++(int)& noexcept // NOLINT(*-dcl21-cpp)
    {
      Address result{*this};
      ++(*this);
      return result;
    }

    /**
     * Overload of postfix decrement operator.
     */
    constexpr Address operator--(int)& noexcept // NOLINT(*-dcl21-cpp)
    {
      Address result{*this};
      --(*this);
      return result;
    }

    /**
     * Explicit conversion operator for accessing the raw value.
     * @return The internal raw value.
     */
    constexpr explicit operator std::uint16_t() const noexcept
    {
      return value_;
    }

  private:
    std::uint16_t value_;
  };

  consteval Address operator "" _addr(unsigned long long value)
  {
    // this makes sure that values greater than 0x0FFF
    // that would become valid after truncating to 16 bits
    // still trigger the range check
    auto const overflow = value>0x0FFF ? 0xF000ull : 0x0ull;
    return Address{static_cast<std::uint16_t>(value | overflow)};
  }
}

#endif // CHIP8_VM_ADDRESS_HXX