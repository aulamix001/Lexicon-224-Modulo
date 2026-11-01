#include "Reverb224.h"
#include <algorithm>
#include <cmath>
namespace lex224 {
void Reverb224::prepare(double sampleRate, int program, int converter,
                        const Settings *initial) {
  hostRate = sampleRate;
  converterProfile = std::clamp(converter, 0, 2);
  sr = converterProfile == 1 ? 20000 : sampleRate;
  clockPhase = 1;
  heldOutput = {};
  previousInput = {};
  converters.prepare(sampleRate);
  activeProgram = std::clamp(program, 0, 8);
  settings = smooth =
      initial ? *initial : programs()[size_t(activeProgram)].defaults;
  hpIn = {};
  hpOut = {};
  tick = 0;
  overloaded = false;
  const auto &lengths = programs()[size_t(activeProgram)].delayTimes;
  for (int ch = 0; ch < 2; ++ch) {
    delays[size_t(ch)].prepare(sr);
    early[size_t(ch)].prepare(sr);
    diffusion[size_t(ch)].prepare(sr, ch);
    for (int i = 0; i < 8; ++i) {
      combs[size_t(ch)][size_t(i)].prepare(sr,
                                           lengths[size_t(i)] + ch * .00119);
      tankDiffusion[size_t(ch)][size_t(i)].prepare(sr, ch * 8 + i + 2);
    }
  }
  chorus.prepare(sr);
  mode.prepare(sr);
  optimization.prepare(sr);
  coefficients();
}
void Reverb224::setSettings(const Settings &s) {
  settings = s;
  settings.program = activeProgram;
}
void Reverb224::coefficients() {
  const auto &p = programs()[size_t(activeProgram)];
  smooth.bass += .15f * (settings.bass - smooth.bass);
  smooth.mid += .15f * (settings.mid - smooth.mid);
  smooth.crossover += .15f * (settings.crossover - smooth.crossover);
  smooth.treble += .15f * (settings.treble - smooth.treble);
  smooth.depth += .15f * (settings.depth - smooth.depth);
  double d = std::clamp(double(settings.predelay), double(p.minDelay),
                        double(p.maxDelay));
  delays[0].setDelay(sr * (d + (p.chamber || p.plate ? 0 : 3)) * .001);
  delays[1].setDelay(sr *
                     std::max(0., d + (p.chamber ? 7
                                       : p.plate ? 0
                                                 : -4)) *
                     .001);
  for (size_t ch = 0; ch < 2; ++ch)
    for (size_t i = 0; i < 8; ++i)
      combs[ch][i].configure(sr, std::clamp(double(smooth.bass), .6, 70.),
                             std::clamp(double(smooth.mid), .6, 70.),
                             std::clamp(double(smooth.crossover), 100., 10900.),
                             std::clamp(double(smooth.treble), 100., 10900.),
                             .0226 + .00148 * double(ch * 8 + i + 2));
}
std::array<float, 4> Reverb224::processCore(float l, float r) {
  if (++tick >= 64) {
    tick = 0;
    coefficients();
  }
  if (!std::isfinite(l))
    l = 0;
  if (!std::isfinite(r))
    r = 0;
  const auto &p = programs()[size_t(activeProgram)];
  if (settings.mono || p.chamber)
    l = r = (l + r) * .5f;
  if (activeProgram == 8) {
    auto result =
        chorus.process(.8f * l + .2f * r, .8f * r + .2f * l, settings.depth,
                       settings.predelay, settings.modeRate, settings.treble);
    overloaded = std::max(std::abs(l), std::abs(r)) > 1;
    if (converterProfile != 2)
      for (auto &output : result)
        output = ConverterModel::fixed16(output);
    return result;
  }
  double gain = optimization.process(std::max(std::abs(l), std::abs(r)),
                                     settings.optimize, settings.decayAmount,
                                     p.nonUniform);
  double modulation = mode.process(settings.mode, settings.modeRate);
  std::array<float, 4> out{};
  overloaded = std::max(std::abs(l), std::abs(r)) > 1;
  const double pole = std::exp(-6.283185307179586 * 20 / sr);
  for (size_t ch = 0; ch < 2; ++ch) {
    float x = ch ? (.8f * r + .2f * l) : (.8f * l + .2f * r);
    double h = pole * (hpOut[ch] + x - hpIn[ch]);
    hpIn[ch] = x;
    hpOut[ch] = h;
    x = delays[ch].process(float(h));
    float e = early[ch].process(x, smooth.depth, int(ch), p);
    float d = diffusion[ch].process(
        x * .6f + e * .4f,
        std::clamp(p.diffusion + (settings.diffusion - .5f) * .2f + modulation,
                   .1, .85));
    float sum = 0, alternate = 0;
    std::array<float, 8> branch{}, feedback{};
    for (size_t i = 0; i < 8; ++i) {
      branch[i] = combs[ch][i].read(modulation * 35 * (i % 2 ? 1 : -1));
      feedback[i] = branch[i];
      sum += branch[i];
      alternate += (i % 2 ? -.6f : 1.f) * branch[i];
    }
    // Orthogonal Walsh-Hadamard scattering: unit energy, all eight paths
    // recirculate.
    for (size_t stride = 1; stride < 8; stride *= 2)
      for (size_t base = 0; base < 8; base += stride * 2)
        for (size_t j = 0; j < stride; ++j) {
          float a = feedback[base + j], b = feedback[base + j + stride];
          feedback[base + j] = a + b;
          feedback[base + j + stride] = a - b;
        }
    for (size_t i = 0; i < 8; ++i) {
      float f = feedback[i] * .35355339059f;
      f = tankDiffusion[ch][i].process(f, .42 + modulation * (i % 2 ? 1 : -1));
      combs[ch][i].push(d * (i % 2 ? .16f : .19f), f, gain,
                        converterProfile != 2);
    }
    float wet = sum * .38f + e * p.earlyGain;
    overloaded = overloaded || std::abs(wet) > 1;
    out[ch] = float(2 * std::tanh(wet / 2));
    out[ch + 2] = float(2 * std::tanh((alternate * .35f + e * .25f) / 2));
    if (converterProfile != 2) {
      out[ch] = ConverterModel::fixed16(out[ch]);
      out[ch + 2] = ConverterModel::fixed16(out[ch + 2]);
    }
  }
  if (p.plate) {
    out[2] = out[1];
    out[3] = out[0];
  }
  return out;
}
std::array<float, 4> Reverb224::process(float l, float r) {
  if (converterProfile != 1)
    return processCore(l, r);
  if (!std::isfinite(l))
    l = 0;
  if (!std::isfinite(r))
    r = 0;
  std::array<float, 2> input{
      {converters.filter(l, 0), converters.filter(r, 1)}};
  clockPhase += sr / hostRate;
  if (clockPhase >= 1) {
    clockPhase -= std::floor(clockPhase);
    double t = std::clamp(1 - clockPhase * hostRate / sr, 0., 1.);
    float a = previousInput[0] + float(t) * (input[0] - previousInput[0]);
    float b = previousInput[1] + float(t) * (input[1] - previousInput[1]);
    heldOutput = processCore(ConverterModel::gainRanging12(a),
                             ConverterModel::gainRanging12(b));
    for (auto &output : heldOutput)
      output = ConverterModel::gainRanging12(output);
  }
  previousInput = input;
  std::array<float, 4> out{};
  for (size_t i = 0; i < 4; ++i)
    out[i] = converters.filter(heldOutput[i], i + 2);
  return out;
}
} // namespace lex224
