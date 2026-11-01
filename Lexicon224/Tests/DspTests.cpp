#include "DSP/Reverb224.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
void require(bool b, const char *message) {
  if (!b)
    throw std::runtime_error(message);
}
double impulse(int program, double sr, float bass = 3, float mid = 2,
               int converter = 0) {
  lex224::Reverb224 e;
  e.prepare(sr, program, converter);
  auto s = lex224::programs()[size_t(program)].defaults;
  s.bass = bass;
  s.mid = mid;
  e.setSettings(s);
  double energy = 0, left = 0, right = 0, late = 0;
  for (int n = 0; n < int(sr * 3); ++n) {
    auto out = e.process(n == 0 ? .5f : 0, 0);
    for (auto v : out)
      require(std::isfinite(v) && std::abs(v) < 2.01f,
              "Unstable impulse response");
    energy += out[0] * out[0] + out[1] * out[1];
    left += out[0] * out[0];
    right += out[1] * out[1];
    if (n > sr * 2)
      late += out[0] * out[0] + out[1] * out[1];
  }
  require(energy > 1e-5, "Silent impulse response");
  require(left > 0 && right > 0, "Missing stereo tail");
  return late;
}
int main() {
  try {
    int count = 0;
    for (double sr : {44100., 48000., 88200., 96000.})
      for (int p = 0; p < 9; ++p)
        for (int profile = 0; profile < 3; ++profile) {
          impulse(p, sr, 3, 2, profile);
          ++count;
        }
    require(impulse(0, 48000, 8, 8) > impulse(0, 48000, .6f, .6f) * 10,
            "RT60 controls do not extend tail");
    for (int p = 0; p < 9; ++p)
      for (int profile = 0; profile < 3; ++profile) {
        lex224::Reverb224 e;
        e.prepare(48000, p, profile);
        auto s = lex224::programs()[size_t(p)].defaults;
        s.bass = s.mid = 70;
        s.treble = 10900;
        s.depth = 71;
        s.predelay = 255;
        e.setSettings(s);
        for (int i = 0; i < 96000; ++i) {
          auto x = e.process(float(std::sin(i * .13)) * 2,
                             float(std::cos(i * .19)) * 2);
          for (auto v : x)
            require(std::isfinite(v) && std::abs(v) <= 3,
                    "Unstable extreme settings");
        }
      }
    auto bright = lex224::programs()[0].defaults;
    bright.treble = 10900;
    bright.mode = false;
    bright.optimize = false;
    auto dark = bright;
    dark.treble = 1000;
    lex224::Reverb224 high, low;
    high.prepare(48000, 0, 2, &bright);
    low.prepare(48000, 0, 2, &dark);
    double highEnergy = 0, lowEnergy = 0;
    float previousHigh = 0, previousLow = 0;
    for (int i = 0; i < 96000; ++i) {
      auto h = high.process(i == 0 ? .5f : 0, 0);
      auto l = low.process(i == 0 ? .5f : 0, 0);
      if (i > 4800) {
        highEnergy += (h[0] - previousHigh) * (h[0] - previousHigh);
        lowEnergy += (l[0] - previousLow) * (l[0] - previousLow);
      }
      previousHigh = h[0];
      previousLow = l[0];
    }
    require(highEnergy > lowEnergy * 1.5,
            "TREBLE DECAY failed to damp the tail's high frequency energy");
    for (int profile = 0; profile < 3; ++profile) {
      lex224::Reverb224 silence;
      silence.prepare(48000, 0, profile);
      for (int i = 0; i < 48000; ++i) {
        auto y = silence.process(0, 0);
        require(y[0] == 0 && y[1] == 0, "Unexcited engine produced noise");
      }
    }
    lex224::Chorus224 stereoChorus;
    stereoChorus.prepare(48000);
    double motion = 0;
    for (int n = 0; n < 96000; ++n) {
      float input = .3f * float(std::sin(n * 6.283185307179586 * 1000 / 48000));
      auto out = stereoChorus.process(input, input, 0, 20, 2, 6000);
      if (n > 24000)
        motion += (out[0] - out[1]) * (out[0] - out[1]);
    }
    require(motion > .01, "Chorus at minimum depth lost stereo modulation");
    lex224::Reverb224 chamber;
    chamber.prepare(48000, 3);
    for (int i = 0; i < 24000; ++i) {
      auto x = chamber.process(i == 0 ? 1.f : 0, i == 0 ? -1.f : 0);
      require(x[0] == 0 && x[1] == 0, "Chamber must mono-sum");
    }
    lex224::Reverb224 plate;
    plate.prepare(48000, 1);
    for (int i = 0; i < 24000; ++i) {
      auto x = plate.process(i == 0 ? 1.f : 0, 0);
      require(x[0] == x[3] && x[1] == x[2], "Plate A=D C=B mapping");
    }
    // An inherent delay must never leak the impulse via an initial zero-delay
    // read head.
    for (int program : {0, 1, 2, 3, 5}) {
      lex224::Reverb224 e;
      e.prepare(48000, program);
      for (int n = 0; n < 480; ++n) {
        auto y = e.process(n == 0 ? .5f : 0, 0);
        require(y[0] == 0 && y[1] == 0, "Inherent predelay leaked at startup");
      }
    }
    // Quantizer error bounds and expected experimental anti-alias filtering.
    for (int i = -10000; i <= 10000; ++i) {
      float x = float(i) / 10001;
      require(std::abs(lex224::ConverterModel::fixed16(x) - x) <=
                  1.f / 65536 + 1e-7f,
              "16-bit quantization error");
      require(std::abs(lex224::ConverterModel::gainRanging12(x) - x) <=
                  1.f / 4094 + 1e-7f,
              "Ranging ADC quantization error");
    }
    lex224::ConverterModel adc;
    adc.prepare(48000);
    double pass = 0, stop = 0;
    for (int n = 0; n < 48000; ++n) {
      float a =
          adc.filter(float(std::sin(n * 6.283185307179586 * 1000 / 48000)), 0);
      float b =
          adc.filter(float(std::sin(n * 6.283185307179586 * 16000 / 48000)), 1);
      if (n > 24000) {
        pass += a * a;
        stop += b * b;
      }
    }
    require(stop < pass * .001,
            "Experimental anti-alias filter rejection too low");
    lex224::PreDelayLine delay;
    delay.prepare(48000);
    delay.setDelay(2400);
    for (int i = 0; i < 100000; ++i) {
      if (i % 997 == 0)
        delay.setDelay(i % 12000);
      float y = delay.process(.25f);
      require(std::isfinite(y) && std::abs(y) <= .25001f,
              "Predelay crossfade bound");
    }
    std::cout
        << count
        << " program/sample-rate impulse cases; decay, extreme stability, "
           "chamber mono, quad pairing and delay sweep passed\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
