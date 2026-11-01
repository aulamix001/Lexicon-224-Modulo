#pragma once
#include <array>
#include <cmath>
#include <vector>
namespace lex224 {
// Designed chorus approximation: three quadrature fractional-delay voices per
// side. Not recovered 224 microcode. The separate algorithm avoids routing
// chorus presets through a hall/plate and exposes distinct depth, tone, rate
// and pre-delay behavior.
class Chorus224 {
  std::array<std::vector<float>, 2> lines;
  size_t write = 0;
  double sr = 48000, phase = 0;
  std::array<double, 2> low{}, hpInput{}, hpOutput{};

public:
  void prepare(double rate) {
    sr = rate;
    for (auto &l : lines)
      l.assign(size_t(sr * .4) + 8, 0);
    write = 0;
    phase = 0;
    low = {};
    hpInput = {};
    hpOutput = {};
  }
  std::array<float, 4> process(float left, float right, float depth,
                               float delay, float rate, float tone) {
    std::array<float, 4> out{};
    phase += (.08 + rate * .035) / sr;
    if (phase >= 1)
      phase -= 1;
    float input[2]{left, right};
    double pole = 1 - std::exp(-6.283185307179586 *
                               std::min(double(tone), sr * .45) / sr);
    for (size_t ch = 0; ch < 2; ++ch) {
      auto &line = lines[ch];
      hpOutput[ch] = std::exp(-6.283185307179586 * 20 / sr) *
                     (hpOutput[ch] + input[ch] - hpInput[ch]);
      hpInput[ch] = input[ch];
      line[write] = float(hpOutput[ch]);
      float sum = 0;
      for (int voice = 0; voice < 3; ++voice) {
        double sweep =
            std::sin(6.283185307179586 * (phase + voice / 3. + ch * .25));
        double samples =
            sr * (.012 + delay * .0006 + (.00045 + .00009 * depth) * sweep +
                  voice * .0041);
        double p = double(write) - samples;
        while (p < 0)
          p += line.size();
        size_t a = size_t(p) % line.size();
        size_t b = (a + 1) % line.size();
        sum += (line[a] + float(p - std::floor(p)) * (line[b] - line[a])) / 3;
      }
      low[ch] += pole * (sum - low[ch]);
      out[ch] = float(low[ch]);
      out[ch + 2] = sum * .75f;
    }
    write = (write + 1) % lines[0].size();
    return out;
  }
};
} // namespace lex224
