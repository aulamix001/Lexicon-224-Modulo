#pragma once
#include "../Programs/ProgramBank.h"
#include "AllpassChain.h"
#include "CombFilter.h"
#include "ConverterModel.h"
#include "DecayOptimization.h"
#include "EarlyReflections.h"
#include "ModeEnhancement.h"
#include "PreDelayLine.h"
#include <array>
namespace lex224 {
class Reverb224 {
  double sr = 48000, hostRate = 48000, clockPhase = 1;
  Settings settings{}, smooth{};
  std::array<PreDelayLine, 2> delays;
  std::array<EarlyReflections, 2> early;
  std::array<AllpassChain, 2> diffusion;
  std::array<std::array<AllpassChain, 8>, 2> tankDiffusion;
  std::array<std::array<CombFilter, 8>, 2> combs;
  ModeEnhancement mode;
  DecayOptimization optimization;
  ConverterModel converters;
  std::array<double, 2> hpIn{}, hpOut{};
  std::array<float, 4> heldOutput{};
  std::array<float, 2> previousInput{};
  int tick = 0, activeProgram = 0, converterProfile = 0;
  bool overloaded = false;
  void coefficients();
  std::array<float, 4> processCore(float, float);

public:
  // Converter profile changes require prepare(), never allocate from
  // setSettings().
  void prepare(double sampleRate, int program = 0, int converter = 0,
               const Settings *initial = nullptr);
  void setSettings(const Settings &);
  std::array<float, 4> process(float left, float right);
  bool overflow() const { return overloaded; }
};
} // namespace lex224
