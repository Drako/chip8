#pragma once

#ifndef CHIP8_VM_LOGGER_HXX
#define CHIP8_VM_LOGGER_HXX

#include <cstdint>
#include <source_location>

namespace chip8 {
  /**
   * Interface representing a Logger.
   *
   * This needs to be implemented by specific frontends.
   */
  struct Logger {
    virtual ~Logger() noexcept = default;

    virtual void debug(char const* message, std::source_location where = std::source_location::current()) = 0;

    virtual void warn(char const* message, std::source_location where = std::source_location::current()) = 0;

    virtual void error(char const* message, std::source_location where = std::source_location::current()) = 0;
  };
}

#endif // CHIP8_VM_LOGGER_HXX
