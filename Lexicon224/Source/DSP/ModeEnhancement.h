#pragma once
#include <cmath>
#include <cstdint>
namespace lex224 {
class ModeEnhancement {
  double phase = 0, step = 0, target = 0, value = 0, slew = 0;
  uint32_t random = 0x2241978;

public:
  void prepare(double sr) {
    phase = 0;
    step = 1 / sr;
    target = value = 0;
    slew = 1 - std::exp(-1 / (sr * .05));
    random = 0x2241978;
  }
  double process(bool enabled, float rate) {
    phase += step * (.35 + .16 * rate);
    if (phase >= 1) {
      phase -= 1;
      random ^= random << 13;
      random ^= random >> 17;
      random ^= random << 5;
      target = (double(random) / 4294967295. * 2 - 1) * .009 /
               std::sqrt(double(rate));
    }
    value += slew * ((enabled ? target : 0) - value);
    return value;
  }
};
} // namespace lex224
