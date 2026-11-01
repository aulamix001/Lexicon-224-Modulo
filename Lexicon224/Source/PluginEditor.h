#pragma once
#include "PluginProcessor.h"
#include "UI/PanelComponents.h"
#include "UI/PresetBrowser.h"
class Lexicon224Editor : public juce::AudioProcessorEditor,
                         private juce::Timer {
public:
  explicit Lexicon224Editor(Lexicon224Processor &);
  ~Lexicon224Editor() override;
  void paint(juce::Graphics &) override;
  void resized() override;
  void visibilityChanged() override {
    if (!isShowing() && window)
      window->setVisible(false);
    if (isShowing() && !browserOpened)
      showPresetBrowser(false);
  }

private:
  void timerCallback() override;
  void showPresetBrowser(bool toggle);
  void set(const char *, float);
  void adjust(const char *, float);
  Lexicon224Processor &processor;
  PanelLook look;
  NumerDisplay display;
  HeadroomMeter meter;
  std::array<juce::TextButton, 8> programs, functions;
  std::array<juce::TextButton, 6> lower;
  std::array<juce::Slider, 6> sliders;
  juce::TextButton open, presetTags{"PRESETS / TAGS"};
  juce::Label presetName;
  bool browserOpened = false;
  std::unique_ptr<PresetWindow> window;
  juce::Component surface;
  int held = -1;
  bool immed = false;
  static constexpr const char *ids[]{"bass",   "mid",   "crossover",
                                     "treble", "depth", "predelay"};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lexicon224Editor)
};
