#include <SDL.h>

#include <call_stack.hxx>
#include <memory.hxx>
#include <processor.hxx>

#include <atomic>
#include <chrono>
#include <fstream>
#include <thread>
#include <vector>

class SdlScreen final : public chip8::Screen {
public:
  void clear() override
  {
    for (auto& line: pixels_) {
      for (auto& pixel: line) {
        pixel = false;
      }
    }
    needs_redraw_ = true;
  }

  bool get_pixel(std::uint8_t const x, std::uint8_t const y) override
  {
    return pixels_[y][x];
  }

  void set_pixel(std::uint8_t const x, std::uint8_t const y, bool const state) override
  {
    pixels_[y][x] = state;
    needs_redraw_ = true;
  }

  void draw_to(SDL_Renderer* renderer)
  {
    if (!needs_redraw_)
      return;
    needs_redraw_ = true;

    std::vector<SDL_Rect> points;
    points.reserve(Screen::WIDTH*Screen::HEIGHT);

    for (auto y = Screen::HEIGHT; y--;) {
      for (auto x = Screen::WIDTH; x--;) {
        if (get_pixel(x, y))
          points.emplace_back(x*10, y*10, 10, 10);
      }
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, points.data(), static_cast<int>(points.size()));
    SDL_RenderPresent(renderer);
  }

private:
  std::array<std::array<bool, 64u>, 32u> pixels_{};
  bool needs_redraw_{true};
};

int main(int argc, char** argv)
{
  if (argc<2) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing argument", "Usage: ./chip_8 [rom]", nullptr);
    return 0;
  }

  std::ifstream rom{argv[1], std::ios::binary | std::ios::ate};
  auto const size = rom.tellg();
  rom.seekg(std::ios::beg);
  std::vector<std::uint8_t> content(size);
  rom.read(reinterpret_cast<char*>(content.data()), size);

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0u);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SdlScreen screen;
  chip8::CallStack call_stack;
  chip8::Memory memory;
  memory.load(chip8::Address{0x200}, content);
  chip8::Processor processor{call_stack, memory, screen};

  std::atomic<bool> run = true;

  std::thread vm_thread{[&run, &processor] {
    using namespace std::chrono_literals;
    auto const intended = 1'300'000ns; // ~700Hz

    auto start = std::chrono::high_resolution_clock::now();
    while (run) {
      auto const end = std::chrono::high_resolution_clock::now();
      auto const elapsed = end-start;
      auto const diff = intended-elapsed;
      if (diff.count()>0) {
        std::this_thread::sleep_for(diff);
      }
      start = std::chrono::high_resolution_clock::now();

      processor.step();
    }
  }};

  while (run) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type==SDL_QUIT)
        run = false;
    }
    processor.step();

    screen.draw_to(renderer);
  }

  vm_thread.join();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
