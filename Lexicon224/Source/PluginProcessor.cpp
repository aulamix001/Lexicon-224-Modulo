#include "PluginProcessor.h"
#include "PluginEditor.h"
namespace {
juce::NormalisableRange<float> skew(float a, float b, float centre) {
  juce::NormalisableRange<float> r(a, b);
  r.setSkewForCentre(centre);
  return r;
}
} // namespace
juce::AudioProcessorValueTreeState::ParameterLayout
Lexicon224Processor::layout() {
  juce::AudioProcessorValueTreeState::ParameterLayout p;
  auto add = [&](const char *id, const char *name,
                 juce::NormalisableRange<float> range, float value,
                 const char *unit) {
    p.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{id, 1}, name, range, value,
        juce::AudioParameterFloatAttributes().withLabel(unit)));
  };
  add("bass", "BASS", skew(.6f, 70, 4), 3, "sec");
  add("mid", "MID", skew(.6f, 70, 4), 2, "sec");
  // Preserve the manual's 540 Hz preset; coarse values remain available via
  // keyboard.
  add("crossover", "CROSSOVER", skew(100, 10900, 1000), 540, "Hz");
  add("treble", "TREBLE DECAY", skew(100, 10900, 4400), 4400, "Hz");
  add("depth", "DEPTH", {0, 71, 1}, 25, "");
  add("predelay", "PRE-DELAY", {0, 255, 1}, 24, "ms");
  add("modeRate", "MODE RATE", {1, 16, 1}, 2, "");
  add("decayAmount", "DECAY AMOUNT", {1, 16, 1}, 5, "");
  add("mix", "DRY/WET", {0, 100, .1f}, 100, "%");
  p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"mode", 1},
                                                   "MODE ENHANCEMENT", true));
  p.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{"optimize", 1}, "DECAY OPTIMIZATION", true));
  p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"mono", 1},
                                                   "MONO BRIDGE", false));
  p.add(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{"converter", 1}, "CONVERTER MODEL",
      juce::StringArray{"16-bit arithmetic (brief)",
                        "Experimental 20 kHz / 12-bit ranging",
                        "Clean reference"},
      0));
  juce::StringArray names;
  for (const auto &pr : lex224::programs())
    names.add(pr.name);
  p.add(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{"program", 1}, "PROGRAM", names, 0));
  return p;
}
Lexicon224Processor::Lexicon224Processor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Stereo / Mono", juce::AudioChannelSet::stereo(), true)
              .withOutput("A / C", juce::AudioChannelSet::stereo(), true)
              .withOutput("B / D", juce::AudioChannelSet::stereo(), false)),
      state(*this, nullptr, "Lexicon224", layout()) {}
bool Lexicon224Processor::isBusesLayoutSupported(const BusesLayout &b) const {
  auto in = b.getMainInputChannelSet();
  auto out = b.getMainOutputChannelSet();
  return (in == juce::AudioChannelSet::stereo() ||
          in == juce::AudioChannelSet::mono()) &&
         out == juce::AudioChannelSet::stereo() &&
         (b.outputBuses.size() == 1 || b.getChannelSet(false, 1).isDisabled() ||
          b.getChannelSet(false, 1) == juce::AudioChannelSet::stereo());
}
lex224::Settings Lexicon224Processor::settings() const {
  lex224::Settings s;
  auto v = [&](const char *k) { return state.getRawParameterValue(k)->load(); };
  s.bass = v("bass");
  s.mid = v("mid");
  s.crossover = v("crossover");
  s.treble = v("treble");
  s.depth = v("depth");
  s.predelay = v("predelay");
  s.modeRate = v("modeRate");
  s.decayAmount = v("decayAmount");
  s.mix = v("mix");
  s.mode = v("mode") > .5f;
  s.optimize = v("optimize") > .5f;
  s.mono = v("mono") > .5f;
  s.converter = juce::jlimit(0, 2, int(v("converter")));
  s.program = juce::jlimit(0, 5, int(v("program")));
  return s;
}
void Lexicon224Processor::prepareToPlay(double sr, int) {
  sampleRateHz = sr;
  auto s = settings();
  for (int i = 0; i < 18; ++i) {
    engines[size_t(i)].prepare(sr, i / 3, i % 3, &s);
    engines[size_t(i)].setSettings(s);
  }
  weights = {};
  weights[size_t(s.program * 3 + s.converter)] = 1;
  wet = s.mix * .01f;
}
void Lexicon224Processor::processBlock(juce::AudioBuffer<float> &buffer,
                                       juce::MidiBuffer &midi) {
  juce::ScopedNoDenormals noDenormals;
  midi.clear();
  const auto s = settings();
  for (auto &e : engines)
    e.setSettings(s);
  auto input = getBusBuffer(buffer, true, 0);
  auto main = getBusBuffer(buffer, false, 0);
  const bool quad =
      getBusCount(false) > 1 && getChannelCountOfBus(false, 1) == 2;
  float peaks[2]{};
  bool over = false;
  const float step = float(1 / (sampleRateHz * .035));
  for (int n = 0; n < buffer.getNumSamples(); ++n) {
    float l = input.getSample(0, n),
          r = input.getSample(input.getNumChannels() > 1 ? 1 : 0, n);
    if (!std::isfinite(l))
      l = 0;
    if (!std::isfinite(r))
      r = 0;
    peaks[0] = juce::jmax(peaks[0], std::abs(l));
    peaks[1] = juce::jmax(peaks[1], std::abs(r));
    float total = 0;
    for (int i = 0; i < 18; ++i) {
      auto &w = weights[size_t(i)];
      w += juce::jlimit(-step, step,
                        (i == s.program * 3 + s.converter ? 1.f : 0.f) - w);
      total += w;
    }
    std::array<float, 4> out{};
    for (int i = 0; i < 18; ++i)
      if (weights[size_t(i)] > 0) {
        auto x = engines[size_t(i)].process(l, r);
        for (size_t c = 0; c < 4; ++c)
          out[c] += x[c] * weights[size_t(i)] / juce::jmax(.00001f, total);
        over = over || engines[size_t(i)].overflow();
      }
    wet += juce::jlimit(-step, step, s.mix * .01f - wet);
    main.setSample(0, n, l * (1 - wet) + out[0] * wet);
    main.setSample(1, n, r * (1 - wet) + out[1] * wet);
    if (quad) {
      auto aux = getBusBuffer(buffer, false, 1);
      aux.setSample(0, n, out[2] * wet);
      aux.setSample(1, n, out[3] * wet);
    }
  }
  for (size_t c = 0; c < 2; ++c)
    meters[c].store(juce::jmax(peaks[c], meters[c].load() * .85f));
  overflow.store(over);
}
int Lexicon224Processor::getCurrentProgram() {
  return int(state.getRawParameterValue("program")->load());
}
const juce::String Lexicon224Processor::getProgramName(int i) {
  return juce::isPositiveAndBelow(i, 6) ? lex224::programs()[size_t(i)].name
                                        : juce::String{};
}
void Lexicon224Processor::setCurrentProgram(int i) {
  if (!juce::isPositiveAndBelow(i, 6))
    return;
  auto s = lex224::programs()[size_t(i)].defaults;
  auto set = [&](const char *key, float x) {
    auto *p = state.getParameter(key);
    p->beginChangeGesture();
    p->setValueNotifyingHost(p->convertTo0to1(x));
    p->endChangeGesture();
  };
  set("bass", s.bass);
  set("mid", s.mid);
  set("crossover", s.crossover);
  set("treble", s.treble);
  set("depth", s.depth);
  set("predelay", s.predelay);
  set("mode", 1);
  set("optimize", 1);
  set("modeRate", 2);
  set("decayAmount", s.decayAmount);
  set("mix", s.mix);
  set("mono", s.mono ? 1.f : 0.f);
  set("converter", float(s.converter));
  set("program", float(i));
  ++presetRevision;
}
void Lexicon224Processor::saveRegister(int i) {
  juce::ScopedLock lock(registerLock);
  registers.store(i, state.copyState());
}
bool Lexicon224Processor::recallRegister(int i) {
  juce::ScopedLock lock(registerLock);
  auto s = registers.recall(i);
  if (!s.isValid())
    return false;
  state.replaceState(s);
  ++presetRevision;
  return true;
}
void Lexicon224Processor::getStateInformation(juce::MemoryBlock &out) {
  juce::ScopedLock lock(registerLock);
  auto s = state.copyState();
  s.addChild(registers.serialize(), -1, nullptr);
  auto xml = s.createXml();
  copyXmlToBinary(*xml, out);
}
void Lexicon224Processor::setStateInformation(const void *data, int bytes) {
  auto xml = getXmlFromBinary(data, bytes);
  if (!xml || !xml->hasTagName("Lexicon224"))
    return;
  auto s = juce::ValueTree::fromXml(*xml);
  juce::ScopedLock lock(registerLock);
  auto r = s.getChildWithName("Registers");
  registers.restore(r);
  s.removeChild(r, nullptr);
  state.replaceState(s);
  ++presetRevision;
}
juce::AudioProcessorEditor *Lexicon224Processor::createEditor() {
  return new Lexicon224Editor(*this);
}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new Lexicon224Processor;
}
