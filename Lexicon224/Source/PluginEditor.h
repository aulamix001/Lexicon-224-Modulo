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
    if (!isShowing() && presetWindow)
      presetWindow->setVisible(false);
  }

private:
  void timerCallback() override;
  void selectRegister(int);
  void armPickup();
  Lexicon224Processor &processor;
  PanelLook look;
  NumerDisplay display;
  HeadroomMeter meter;
  SliderPanel panel;
  ProgramButtons programs;
  ModeButtons modes;
  RegisterButtons registers;
  juce::TextButton presetButton{"PRESETS"};
  std::unique_ptr<PresetWindow> presetWindow;
  juce::Slider mix, rate, amount;
  juce::ComboBox converter;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
      converterAttachment;
  juce::ToggleButton mono{"MONO BRIDGE"};
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      mixAttachment, rateAttachment, amountAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
      monoAttachment;
  std::array<bool, 6> active{{true, true, true, true, true, true}}, dragging{};
  std::array<double, 6> previous{};
  unsigned revision = 0;
  bool storing = false, shift = false, call = true;
  int bank = 0, held = -1;
  juce::String status = "CALL · factory presets";
  static constexpr const char *ids[]{"bass",   "mid",   "crossover",
                                     "treble", "depth", "predelay"};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lexicon224Editor)
};
