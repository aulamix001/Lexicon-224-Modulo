#pragma once
#include "ConverterModel.h"
#include <algorithm>
#include <cmath>
#include <vector>
namespace lex224 {
// A delay branch in a coupled orthogonal tank, rather than an independent comb.
class CombFilter {
  std::vector<float> data;
  size_t pos = 0;
  double low = 0, damped = 0, gb = 0, gm = 0, xc = 0, tc = 0;

public:
  void prepare(double sr, double seconds) {
    data.assign(size_t(sr * seconds) + 3, 0);
    pos = 0;
    low = damped = 0;
  }
  void configure(double sr, double bass, double mid, double crossover,
                 double treble, double diffusionSeconds) {
    double d = double(data.size() - 2) / sr + diffusionSeconds;
    gb = std::pow(10., -3 * d / bass);
    gm = std::pow(10., -3 * d / mid);
    xc = 1 - std::exp(-6.283185307179586 * std::min(crossover, sr * .45) / sr);
    tc = 1 - std::exp(-6.283185307179586 * std::min(treble, sr * .45) / sr);
  }
  float read(double modulation) const {
    double offset = 1. + modulation;
    int integral = int(std::floor(offset));
    double fraction = offset - integral;
    auto i = (pos + size_t(integral) + data.size()) % data.size();
    auto j = (i + 1) % data.size();
    return data[i] + float(fraction) * (data[j] - data[i]);
  }
  void push(float x, double feedback, double scale, bool quantize) {
    low += xc * (feedback - low);
    double shaped = gb * low + gm * (feedback - low);
    damped += tc * (shaped - damped);
    double y = x + damped * std::clamp(scale, 0., 1.);
    float v = float(
        std::abs(y) <= 1
            ? y
            : std::copysign(1 + .5 * std::tanh(2 * (std::abs(y) - 1)), y));
    data[pos] = quantize ? ConverterModel::fixed16(v) : v;
    pos = (pos + 1) % data.size();
  }
};
} // namespace lex224
