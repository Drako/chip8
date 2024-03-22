#include <SDL.h>

#include <call_stack.hxx>
#include <memory.hxx>
#include <processor.hxx>

#include <atomic>
#include <chrono>
#include <fstream>
#include <thread>
#include <vector>

class SdlLogger final : public chip8::Logger {

  void debug(char const* message, std::source_location const where) override
  {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s:%d:%d: %s",
        where.file_name(),
        static_cast<int>(where.line()),
        static_cast<int>(where.column()),
        message
    );
  }

  void warn(char const* message, std::source_location const where) override
  {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s:%d:%d: %s",
        where.file_name(),
        static_cast<int>(where.line()),
        static_cast<int>(where.column()),
        message
    );
  }

  void error(char const* message, std::source_location const where) override
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s:%d:%d: %s",
        where.file_name(),
        static_cast<int>(where.line()),
        static_cast<int>(where.column()),
        message
    );
  }
};

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
    needs_redraw_ = false;

    std::array<SDL_Rect, Screen::WIDTH*Screen::HEIGHT> points{};
    int num_points = 0;
    for (auto y = Screen::HEIGHT; y--;) {
      for (auto x = Screen::WIDTH; x--;) {
        if (get_pixel(x, y))
          points[num_points++] = SDL_Rect{x*20, y*20, 20, 20};
      }
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, points.data(), num_points);
    SDL_RenderPresent(renderer);
  }

private:
  std::array<std::array<bool, WIDTH>, HEIGHT> pixels_{};
  bool needs_redraw_{true};
};

int main(int argc, char** argv)
{
  if (argc<2) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing argument", "Usage: ./chip_8 [rom] {delay-in-ms=1000}",
        nullptr);
    return 0;
  }

  auto const delay = argc>2 ? std::stoi(argv[2]) : 1000;
  bool show_debug_log = false;

  std::ifstream rom{argv[1], std::ios::binary | std::ios::ate};
  auto const size = rom.tellg();
  rom.seekg(std::ios::beg);
  std::vector<std::uint8_t> content(size);
  rom.read(reinterpret_cast<char*>(content.data()), size);

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 640, 0u);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SdlScreen screen;
  SdlLogger logger;
  chip8::CallStack call_stack;
  chip8::Memory memory;
  memory.load(chip8::Processor::CODE_START, content);
  chip8::Config const config = {
      .register_rw_modifies_i = true,
      .shift_takes_value_from_vy = true,
      .use_vx_for_offset_jump = false,
  };
  chip8::Processor processor{config, call_stack, memory, screen, logger};

  auto const timer_updater = SDL_AddTimer(delay, [](std::uint32_t const interval, void* ctx) -> std::uint32_t {
    auto const processor = reinterpret_cast<chip8::Processor*>(ctx);
    processor->update_timers();
    return interval;
  }, &processor);

  std::atomic<bool> run = true;

  std::thread vm_thread{[&run, &processor] {
    using namespace std::chrono_literals;
    auto const intended = 1'428'000ns; // ~700Hz

    auto start = std::chrono::high_resolution_clock::now();
    while (run) {
      auto const end = std::chrono::high_resolution_clock::now();
      auto const elapsed = end-start;
      auto const diff = intended-elapsed;
      if (diff.count()>0) {
        std::this_thread::sleep_for(diff);
      }
      start = std::chrono::high_resolution_clock::now();

      if (!processor.step())
        run = false;
    }
  }};

  auto const toggle_key = [&processor](SDL_Scancode const scancode, bool pressed) {
    switch (scancode) {
    default:
      break;
    case SDL_SCANCODE_1:
      processor.toggle_key(0x1, pressed);
      break;
    case SDL_SCANCODE_2:
      processor.toggle_key(0x2, pressed);
      break;
    case SDL_SCANCODE_3:
      processor.toggle_key(0x3, pressed);
      break;
    case SDL_SCANCODE_4:
      processor.toggle_key(0xC, pressed);
      break;
    case SDL_SCANCODE_Q:
      processor.toggle_key(0x4, pressed);
      break;
    case SDL_SCANCODE_W:
      processor.toggle_key(0x5, pressed);
      break;
    case SDL_SCANCODE_E:
      processor.toggle_key(0x6, pressed);
      break;
    case SDL_SCANCODE_R:
      processor.toggle_key(0xD, pressed);
      break;
    case SDL_SCANCODE_A:
      processor.toggle_key(0x7, pressed);
      break;
    case SDL_SCANCODE_S:
      processor.toggle_key(0x8, pressed);
      break;
    case SDL_SCANCODE_D:
      processor.toggle_key(0x9, pressed);
      break;
    case SDL_SCANCODE_F:
      processor.toggle_key(0xE, pressed);
      break;
    case SDL_SCANCODE_Z:
      processor.toggle_key(0xA, pressed);
      break;
    case SDL_SCANCODE_X:
      processor.toggle_key(0x0, pressed);
      break;
    case SDL_SCANCODE_C:
      processor.toggle_key(0xB, pressed);
      break;
    case SDL_SCANCODE_V:
      processor.toggle_key(0xF, pressed);
      break;
    }
  };

  while (run) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type==SDL_QUIT)
        run = false;
      else if (evt.type==SDL_KEYDOWN) {
        toggle_key(evt.key.keysym.scancode, true);
      }
      else if (evt.type==SDL_KEYUP) {
        if (evt.key.keysym.scancode==SDL_SCANCODE_L) {
          show_debug_log = !show_debug_log;
          SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION,
              show_debug_log ? SDL_LOG_PRIORITY_DEBUG : SDL_LOG_PRIORITY_INFO);
        }
        else {
          toggle_key(evt.key.keysym.scancode, false);
        }
      }
    }

    screen.draw_to(renderer);
  }

  vm_thread.join();

  SDL_RemoveTimer(timer_updater);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
