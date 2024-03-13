#pragma once

#ifndef CHIP8_VM_SCREEN_HXX
#define CHIP8_VM_SCREEN_HXX

#include <cstdint>

namespace chip8 {
  /**
   * Interface representing a screen.
   *
   * This needs to be implemented by specific frontends.
   */
  struct Screen {
    static std::uint8_t constexpr WIDTH = 64;
    static std::uint8_t constexpr HEIGHT = 32;

    virtual ~Screen() noexcept = default;

    /**
     * Clear the screen.
     */
    virtual void clear() = 0;

    /**
     * Check whether the pixel at the given position is turned on.
     *
     * @param x The x coordinate of the pixel.
     * @param y The y coordinate of the pixel.
     * @return true, if the pixel is turned on, false otherwise.
     */
    virtual bool get_pixel(std::uint8_t x, std::uint8_t y) = 0;

    virtual void set_pixel(std::uint8_t x, std::uint8_t y, bool state) = 0;
  };
}

#endif // CHIP8_VM_SCREEN_HXX
