#pragma once
#include <algorithm>
#include <array>
#include <cmath>
namespace lex224 {
// These profiles are hypotheses, NOT verified converter schematics.
// 0: brief's 16-bit arithmetic. 1: experimental 20 kHz / gain-ranging ADC.
// 2: full precision reference, for null/error and noise measurements.
class ConverterModel {
  struct Biquad {
    double b0 = 0, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
    void prepare(double sr, double hz, double q) {
      double w = 6.283185307179586 * hz / sr, c = std::cos(w),
             a = std::sin(w) / (2 * q), den = 1 + a;
      b0 = (1 - c) / (2 * den);
      b1 = (1 - c) / den;
      b2 = b0;
      a1 = -2 * c / den;
      a2 = (1 - a) / den;
      z1 = z2 = 0;
    }
    float process(float x) {
      double y = b0 * x + z1;
      z1 = b1 * x - a1 * y + z2;
      z2 = b2 * x - a2 * y;
      return float(y);
    }
  };
  std::array<std::array<Biquad, 3>, 6> filters;

public:
  void prepare(double rate) {
    for (auto &bank : filters) {
      int i = 0;
      for (auto &f : bank) {
        constexpr double q[]{.5176380902, .7071067812, 1.9318516526};
        f.prepare(rate, std::min(8000., rate * .4), q[i++]);
      }
    }
  }
  float filter(float x, size_t channel) {
    for (auto &f : filters[channel])
      x = f.process(x);
    return x;
  }
  static float fixed16(float x) {
    return float(std::round(std::clamp(double(x), -4., 4.) * 32768.) / 32768.);
  }
  static float gainRanging12(float x) {
    // Four binary gain ranges; threshold placement and mantissa width are
    // uncalibrated.
    float gain = 1;
    while (gain < 8 && std::abs(x) * gain < .5f)
      gain *= 2;
    return float(std::round(std::clamp(double(x * gain), -1., 1.) * 2047.) /
                 (2047. * gain));
  }
};
} // namespace lex224
