#include "DSP/Reverb224.h"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
// Portable IEEE-float stereo WAV, intended for offline analysis/AB comparisons.
static void u32(std::ostream &out, uint32_t n) {
  for (int i = 0; i < 4; ++i)
    out.put(char((n >> (i * 8)) & 255));
}
static void u16(std::ostream &out, uint16_t n) {
  out.put(char(n & 255));
  out.put(char(n >> 8));
}
int main(int argc, char **argv) {
  const std::filesystem::path dir = argc > 1 ? argv[1] : "responses";
  std::filesystem::create_directories(dir);
  constexpr int rate = 48000, samples = rate * 12;
  for (int profile = 0; profile < 3; ++profile)
    for (int program = 0; program < 6; ++program) {
      lex224::Reverb224 engine;
      engine.prepare(rate, program, profile);
      std::ofstream out(dir / ("program" + std::to_string(program + 1) +
                               "_profile" + std::to_string(profile) + ".wav"),
                        std::ios::binary);
      out.write("RIFF", 4);
      u32(out, 36 + samples * 8);
      out.write("WAVEfmt ", 8);
      u32(out, 16);
      u16(out, 3);
      u16(out, 2);
      u32(out, rate);
      u32(out, rate * 8);
      u16(out, 8);
      u16(out, 32);
      out.write("data", 4);
      u32(out, samples * 8);
      for (int i = 0; i < samples; ++i) {
        auto x = engine.process(i == 0 ? .5f : 0, 0);
        out.write(reinterpret_cast<const char *>(x.data()), 8);
      }
      if (!out)
        return 1;
    }
  std::cout << "18 stereo impulse responses written to " << dir << '\n';
}
