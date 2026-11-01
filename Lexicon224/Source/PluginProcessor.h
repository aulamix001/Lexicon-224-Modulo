#pragma once
#include "DSP/Reverb224.h"
#include "Programs/RegisterStore.h"
#include <JuceHeader.h>
class Lexicon224Processor : public juce::AudioProcessor {
public:
  Lexicon224Processor();
  void prepareToPlay(double, int) override;
  void releaseResources() override {}
  using juce::AudioProcessor::processBlock;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
  bool isBusesLayoutSupported(const BusesLayout &) const override;
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }
  const juce::String getName() const override { return "Lexicon224"; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 70; }
  int getNumPrograms() override { return 55; }
  int getCurrentProgram() override;
  int getCurrentAlgorithm() const;
  void selectAlgorithm(int);
  void loadPhotoPreset(const juce::var &);
  void setCurrentProgram(int) override;
  const juce::String getProgramName(int) override;
  void changeProgramName(int, const juce::String &) override {}
  void getStateInformation(juce::MemoryBlock &) override;
  void setStateInformation(const void *, int) override;
  void saveRegister(int);
  bool recallRegister(int);
  juce::AudioProcessorValueTreeState state;
  std::array<std::atomic<float>, 2> meters{{0.f, 0.f}};
  std::atomic<bool> overflow{false};
  std::atomic<unsigned> presetRevision{0};

private:
  static juce::AudioProcessorValueTreeState::ParameterLayout layout();
  lex224::Settings settings() const;
  std::array<lex224::Reverb224, 27> engines;
  std::array<float, 27> weights{};
  double sampleRateHz = 48000;
  float wet = 1;
  RegisterStore registers;
  juce::CriticalSection registerLock;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lexicon224Processor)
};
