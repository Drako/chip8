#include <SDL.h>

#include <call_stack.hxx>
#include <memory.hxx>
#include <processor.hxx>

#include <fstream>
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
  }

  bool get_pixel(std::uint8_t const x, std::uint8_t const y) override
  {
    return pixels_[y][x];
  }

  void set_pixel(std::uint8_t const x, std::uint8_t const y, bool const state) override
  {
    pixels_[y][x] = state;
  }

  void draw_to(SDL_Renderer* renderer)
  {
    std::vector<SDL_Rect> points;
    points.reserve(Screen::WIDTH*Screen::HEIGHT);

    for (auto y = Screen::HEIGHT; y--;) {
      for (auto x = Screen::WIDTH; x--;) {
        if (get_pixel(x, y))
          points.emplace_back(x*10, y*10, 10, 10);
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, points.data(), static_cast<int>(points.size()));
  }

private:
  std::array<std::array<bool, 64u>, 32u> pixels_{};
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
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_CreateWindowAndRenderer(640, 320, 0u, &window, &renderer);

  SdlScreen screen;
  chip8::CallStack call_stack;
  chip8::Memory memory;
  memory.load(chip8::Address{0x200}, content);
  chip8::Processor processor{call_stack, memory, screen};

  auto const timer = SDL_AddTimer(1u, [](std::uint32_t const, void* ctx) -> std::uint32_t {
    auto const p = reinterpret_cast<chip8::Processor*>(ctx);
    SDL_Log("Step");
    p->step();
    return 1u;
  }, &processor);

  bool run = true;
  while (run) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type==SDL_QUIT)
        run = false;
    }

    SDL_Log("Render");
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    screen.draw_to(renderer);
    SDL_RenderPresent(renderer);
  }

  SDL_RemoveTimer(timer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
