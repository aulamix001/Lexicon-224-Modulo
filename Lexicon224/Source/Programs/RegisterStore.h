#pragma once
#include <JuceHeader.h>
#include <array>
class RegisterStore {
  std::array<juce::ValueTree, 36> slots;

public:
  void store(int slot, const juce::ValueTree &state);
  juce::ValueTree recall(int slot) const;
  juce::ValueTree serialize() const;
  void restore(const juce::ValueTree &);
};
