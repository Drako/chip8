#pragma once

#ifndef CHIP8_VM_CALLSTACK_HXX
#define CHIP8_VM_CALLSTACK_HXX

#include <optional>
#include <stack>

#include "address.hxx"

namespace chip8 {
  class CallStack final {
  public:
    /**
     * Push an address onto the call stack.
     *
     * @param address The address to be pushed.
     */
    void push(Address address);

    /**
     * Pop the most recently pushed address from the stack and return it.
     *
     * @return The most recently pushed address.
     */
    std::optional<Address> pop();

    /**
     * Get the most recently pushed address without removing it from the call stack.
     *
     * @return The most recently pushed address.
     */
    [[nodiscard]] std::optional<Address> top() const;

    /**
     * Get the number of address on the call stack.
     *
     * @return The number of addresses on the call stack.
     */
    [[nodiscard]] std::size_t size() const;

    /**
     * Check whether the call stack is empty.
     *
     * @return true, if the stack is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const;

  private:
    std::stack<Address> stack_{};
  };
}

#endif // CHIP8_VM_CALLSTACK_HXX
