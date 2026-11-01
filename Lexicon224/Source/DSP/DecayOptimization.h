#pragma once
#include <algorithm>
#include <cmath>
namespace lex224 {
class DecayOptimization {
  double fast = 0, slow = 0, release = 0, age = 1, sr = 48000;

public:
  void prepare(double rate) {
    sr = rate;
    fast = slow = 0;
    age = 1;
    release = std::exp(-1 / (sr * .12));
  }
  double process(float x, bool enabled, float amount, float nonUniform) {
    double a = std::abs(x);
    fast = std::max(a, fast * release);
    slow += (a - slow) * (1 - std::exp(-1 / (sr * .25)));
    if (a > .01 && a > slow * 4 && age > .08)
      age = 0;
    age += 1 / sr;
    double onset = nonUniform * std::max(0., 1 - age / .22);
    double adaptive =
        enabled ? .015 * (amount / 16.) * std::clamp(fast - slow, 0., 1.) : 0;
    return std::clamp(1 - onset - adaptive, .5, 1.);
  }
};
} // namespace lex224
