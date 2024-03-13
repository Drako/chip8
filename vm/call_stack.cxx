#include "call_stack.hxx"

namespace chip8 {
  void CallStack::push(Address const address)
  {
    stack_.push(address);
  }

  std::optional<Address> CallStack::pop()
  {
    if (stack_.empty()) {
      return {};
    }
    else {
      auto const result = stack_.top();
      stack_.pop();
      return result;
    }
  }

  std::optional<Address> CallStack::top() const
  {
    return stack_.empty() ? std::optional<Address>{} : std::optional<Address>{stack_.top()};
  }

  std::size_t CallStack::size() const
  {
    return stack_.size();
  }

  bool CallStack::empty() const
  {
    return stack_.empty();
  }
}
