#pragma once
#include "../Programs/ProgramBank.h"
#include <array>
#include <cmath>
#include <vector>
namespace lex224 {
class EarlyReflections {
  std::vector<float> data;
  size_t p = 0;
  double sr = 48000;

public:
  void prepare(double rate) {
    sr = rate;
    data.assign(size_t(sr * .4) + 2, 0);
    p = 0;
  }
  float process(float x, float depth, int channel, const Program &program) {
    const auto &taps = program.tapTimes;
    data[p] = x;
    float sum = 0;
    for (size_t i = 0; i < taps.size(); ++i) {
      double t = taps[i] + (i ? channel * .0013 : 0);
      auto d = size_t(t * sr) % data.size();
      float z = float(i) / 7;
      float g = (1 - z) * (1 - depth / 71.f) + z * depth / 71.f;
      if (program.chamber)
        g = .8f;
      sum +=
          data[(p + data.size() - d) % data.size()] * g * program.tapGains[i];
    }
    p = (p + 1) % data.size();
    return sum * .32f;
  }
};
} // namespace lex224
