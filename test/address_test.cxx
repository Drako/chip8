#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include <address.hxx>

using namespace Catch::Matchers;
using namespace chip8;

TEST_CASE("Address", "[chip8][address]")
{
  SECTION("Addresses bigger than 12 bits trigger InvalidAddressException") {
    REQUIRE_THROWS_MATCHES(Address{0xF000}, InvalidAddressException,
        Message("Provided address does not fit in 12 bits."));
  }

  SECTION("Address in valid range can be constructed") {
    REQUIRE(0x0FEE_addr==Address{0x0FEE});
  }

  SECTION("Address can explicitly be converted to raw value") {
    REQUIRE(static_cast<std::uint16_t>(0x0BAD_addr)==0x0BAD);
  }

  SECTION("Addresses can be created from bigger values by truncation") {
    REQUIRE(Address{0xFEED, Address::Truncate{}}==0x0EED_addr);
  }
}
