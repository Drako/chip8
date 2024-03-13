#include <catch2/catch_test_macros.hpp>

#include <call_stack.hxx>

using namespace chip8;

TEST_CASE("CallStack", "[chip8][call_stack]")
{
  CallStack stack{};

  SECTION("Addresses can be pushed and popped") {
    stack.push(0xBAD_addr);
    CHECK(stack.top()==0xBAD_addr);
    stack.push(0xFEE_addr);
    CHECK(stack.top()==0xFEE_addr);
    CHECK(stack.pop()==0xFEE_addr);
    CHECK(stack.top()==0xBAD_addr);
    CHECK(stack.pop()==0xBAD_addr);
  }

  SECTION("Size and emptiness can be checked") {
    CHECK(stack.empty());
    CHECK(stack.size()==0); // NOLINT(*-container-size-empty)
    stack.push(0xF00_addr);
    CHECK(!stack.empty());
    CHECK(stack.size()==1);
    stack.pop();
    CHECK(stack.empty());
    CHECK(stack.size()==0); // NOLINT(*-container-size-empty)
  }

  SECTION("Empty stack returns empty optionals") {
    CHECK(!stack.top().has_value());
    CHECK(!stack.pop().has_value());
  }
}
