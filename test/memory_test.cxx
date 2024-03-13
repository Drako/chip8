#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include <memory.hxx>

using namespace Catch::Matchers;
using namespace chip8;

TEST_CASE("Memory", "[chip8][memory]")
{
  Memory mem{};

  SECTION("Memory is 4kiB in size") {
    REQUIRE(sizeof(Memory)==4096u);
  }

  SECTION("Memory is zero filled by default") {
    for (std::uint16_t n = 0x0; n<0x1000u; ++n)
      CHECK(mem[Address{n, Address::Truncate{}}]==0u);
  }

  SECTION("Data can be loaded into memory") {
    std::array<std::uint8_t, 4u> const data{0x0B, 0xAD, 0xCA, 0xFE};
    REQUIRE_NOTHROW(mem.load(0x200_addr, data));

    for (std::uint16_t n = 0; n<4; ++n)
      CHECK(mem[Address{static_cast<std::uint16_t>(0x200u+n), Address::Truncate{}}]==data[n]);
  }

  SECTION("Loading data to end of memory works") {
    REQUIRE_NOTHROW(mem.load(0xFFF_addr, std::ranges::single_view{23}));
    REQUIRE(mem[0xFFF_addr]==23);
  }

  SECTION("Loading over the end of memory results in an exception") {
    std::array<std::uint8_t, 2u> const data{23, 42};
    REQUIRE_THROWS_MATCHES(mem.load(0xFFF_addr, data), MemoryOverflowException,
        Message("Trying to load more data than fits in memory"));
  }
}
