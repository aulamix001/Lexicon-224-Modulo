#pragma once
#include <array>
namespace lex224 {
struct Settings {
  float bass = 3, mid = 2, crossover = 540, treble = 4400, depth = 25,
        predelay = 24;
  float modeRate = 2, decayAmount = 5, mix = 100;
  bool mode = true, optimize = true, mono = false;
  int program = 0;
  int converter = 0;
  float diffusion = .5f;
};
struct Program {
  const char *name;
  Settings defaults;
  float minDelay, maxDelay, size, diffusion, earlyGain, nonUniform;
  bool plate, chamber;
  std::array<double, 8> delayTimes{};
  std::array<double, 8> tapTimes{};
  std::array<float, 8> tapGains{};
};
const std::array<Program, 9> &programs();
} // namespace lex224
