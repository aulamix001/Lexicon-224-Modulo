#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
namespace lex224 {
// Two stationary read heads crossfade: changing delay does not sweep pitch.
class PreDelayLine {
  std::vector<float> data;
  size_t pos = 0;
  bool initialized = false;
  double current = 0, target = 0, pending = 0, fade = 1, increment = 1;
  float read(double delay) const {
    double p = double(pos) - delay;
    while (p < 0)
      p += double(data.size());
    auto i = size_t(p) % data.size();
    auto j = (i + 1) % data.size();
    return data[i] + float(p - std::floor(p)) * (data[j] - data[i]);
  }

public:
  void prepare(double sr) {
    data.assign(size_t(sr * .6) + 4, 0);
    pos = 0;
    initialized = false;
    current = target = pending = 0;
    fade = 1;
    increment = 1 / (sr * .035);
  }
  void setDelay(double samples) {
    pending = std::clamp(samples, 0.0, double(data.size() - 2));
    if (!initialized) {
      current = target = pending;
      fade = 1;
      initialized = true;
    }
  }
  float process(float x) {
    if (fade >= 1 && std::abs(pending - target) > .01) {
      current = target;
      target = pending;
      fade = 0;
    }
    data[pos] = x;
    float y = read(current) * float(1 - fade) + read(target) * float(fade);
    fade = std::min(1.0, fade + increment);
    pos = (pos + 1) % data.size();
    return y;
  }
};
} // namespace lex224
