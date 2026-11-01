#pragma once
#include <array>
#include <vector>
namespace lex224 {
class AllpassChain {
  struct Stage {
    std::vector<float> data;
    size_t p = 0;
  };
  std::array<Stage, 4> stages;

public:
  void prepare(double sr, int channel) {
    int i = 0;
    for (auto &s : stages) {
      s.data.assign(size_t(sr * (.0031 + .0017 * i + .00037 * channel)), 0);
      s.p = 0;
      ++i;
    }
  }
  float process(float x, double g) {
    for (auto &s : stages) {
      float d = s.data[s.p];
      float y = d - float(g) * x;
      s.data[s.p] = x + float(g) * y;
      s.p = (s.p + 1) % s.data.size();
      x = y;
    }
    return x;
  }
};
} // namespace lex224
