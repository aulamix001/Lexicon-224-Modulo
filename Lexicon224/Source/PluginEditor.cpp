#include "PluginEditor.h"
Lexicon224Editor::Lexicon224Editor(Lexicon224Processor &p)
    : AudioProcessorEditor(&p), processor(p) {
  addAndMakeVisible(presetButton);
  presetButton.onClick = [this] {
    if (!presetWindow)
      presetWindow = std::make_unique<PresetWindow>(processor);
    if (presetWindow->isVisible())
      presetWindow->setVisible(false);
    else
      presetWindow->showBeside(*this);
  };
  setLookAndFeel(&look);
  setSize(535, 960);
  addAndMakeVisible(display);
  addAndMakeVisible(meter);
  const char *labels[]{"BASS",         "MID",   "CROSSOVER",
                       "TREBLE DECAY", "DEPTH", "PRE-DELAY"};
  for (size_t i = 0; i < 6; ++i) {
    auto &s = panel.sliders[i];
    auto *param = processor.state.getParameter(ids[i]);
    auto range = param->getNormalisableRange();
    s.setNormalisableRange(juce::NormalisableRange<double>(
        range.start, range.end, range.interval, range.skew));
    s.setSliderStyle(juce::Slider::LinearVertical);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setValue(processor.state.getRawParameterValue(ids[i])->load(),
               juce::dontSendNotification);
    previous[i] = s.getValue();
    s.setTooltip(labels[i]);
    addAndMakeVisible(s);
    s.onDragStart = [this, i] {
      dragging[i] = true;
      held = int(i);
      processor.state.getParameter(ids[i])->beginChangeGesture();
    };
    s.onDragEnd = [this, i] {
      dragging[i] = false;
      held = -1;
      processor.state.getParameter(ids[i])->endChangeGesture();
    };
    s.onValueChange = [this, i] {
      auto &slider = panel.sliders[i];
      double value = slider.getValue(),
             target = processor.state.getRawParameterValue(ids[i])->load();
      if (!active[i]) {
        double now = slider.valueToProportionOfLength(value),
               before = slider.valueToProportionOfLength(previous[i]),
               goal = slider.valueToProportionOfLength(target);
        if ((now - goal) * (before - goal) <= 0 || std::abs(now - goal) < .015)
          active[i] = true;
      }
      if (active[i]) {
        auto *parameter = processor.state.getParameter(ids[i]);
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(float(value)));
      }
      previous[i] = value;
    };
    auto &b = panel.displayButtons[i];
    b.setButtonText(labels[i]);
    b.onStateChange = [this, i] {
      if (panel.displayButtons[i].isDown())
        held = int(i);
      else if (held == int(i))
        held = -1;
    };
    addAndMakeVisible(b);
  }
  const char *shortNames[]{"1  SM HALL B", "2  VOC PLATE",  "3  LG HALL B",
                           "4  CHAMBER",   "5  PERC PLATE", "6  SM HALL A",
                           "7  MODE ENH",  "8  DECAY OPT"};
  for (int i = 0; i < 8; ++i) {
    auto &b = programs.buttons[size_t(i)];
    b.setButtonText(shortNames[i]);
    addAndMakeVisible(b);
    b.onClick = [this, i] {
      if (shift) {
        int slot = 4 + bank * 8 + i;
        if (storing) {
          processor.saveRegister(slot);
          status = "Stored bank " + juce::String(bank + 1) + " / " +
                   juce::String(i + 1);
        } else
          status = processor.recallRegister(slot) ? "Recalled extended register"
                                                  : "Empty extended register";
        return;
      }
      if (i < 6) {
        if (call)
          processor.setCurrentProgram(i);
        else {
          auto *p2 = processor.state.getParameter("program");
          p2->beginChangeGesture();
          p2->setValueNotifyingHost(p2->convertTo0to1(float(i)));
          p2->endChangeGesture();
        }
        status = processor.getProgramName(i);
      } else {
        const char *id = i == 6 ? "mode" : "optimize";
        auto *p2 = processor.state.getParameter(id);
        p2->beginChangeGesture();
        p2->setValueNotifyingHost(p2->getValue() > .5f ? 0.f : 1.f);
        p2->endChangeGesture();
      }
    };
  }
  const char *modeNames[]{"IMMED", "SET", "CALL", "SHIFT", "FUNCTION"};
  for (size_t i = 0; i < 5; ++i) {
    auto &b = modes.buttons[i];
    b.setButtonText(modeNames[i]);
    addAndMakeVisible(b);
    b.onClick = [this, i] {
      if (i == 0) {
        active.fill(true);
        call = false;
        for (size_t j = 0; j < 6; ++j) {
          auto *p2 = processor.state.getParameter(ids[j]);
          p2->beginChangeGesture();
          p2->setValueNotifyingHost(
              p2->convertTo0to1(float(panel.sliders[j].getValue())));
          p2->endChangeGesture();
        }
        status = "IMMED · sliders active";
      }
      if (i == 1) {
        storing = !storing;
        status = storing ? "SET · select register" : "Recall registers";
      }
      if (i == 2) {
        call = true;
        processor.setCurrentProgram(processor.getCurrentProgram());
        status = "CALL · recommended settings";
      }
      if (i == 3) {
        shift = !shift;
        status = shift ? "SHIFT · A–D chooses bank, 1–8 selects slot"
                       : "Main registers A–D";
      }
      if (i == 4) {
        shift = storing = false;
        held = -1;
        status = "Normal operation";
      }
    };
  }
  for (int i = 0; i < 4; ++i) {
    auto &b = registers.buttons[size_t(i)];
    b.setButtonText(juce::String::charToString(juce::juce_wchar('A' + i)));
    b.onClick = [this, i] { selectRegister(i); };
    addAndMakeVisible(b);
  }
  auto small = [this](juce::Slider &s) {
    s.setSliderStyle(juce::Slider::LinearHorizontal);
    s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 42, 20);
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff2c2c27));
    addAndMakeVisible(s);
  };
  small(mix);
  small(rate);
  small(amount);
  addAndMakeVisible(mono);
  mixAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.state, "mix", mix);
  rateAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.state, "modeRate", rate);
  amountAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.state, "decayAmount", amount);
  monoAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
          p.state, "mono", mono);
  converter.addItemList(
      juce::StringArray{"16-bit arithmetic (brief)",
                        "Experimental 20 kHz / 12-bit ranging",
                        "Clean reference"},
      1);
  addAndMakeVisible(converter);
  converterAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
          p.state, "converter", converter);
  revision = p.presetRevision.load();
  startTimerHz(30);
}
Lexicon224Editor::~Lexicon224Editor() {
  stopTimer();
  setLookAndFeel(nullptr);
}
void Lexicon224Editor::armPickup() {
  active.fill(false);
  for (size_t i = 0; i < 6; ++i)
    previous[i] = panel.sliders[i].getValue();
}
void Lexicon224Editor::selectRegister(int i) {
  if (shift) {
    bank = i;
    status = "Extended bank " +
             juce::String::charToString(juce::juce_wchar('A' + i)) +
             " · select 1–8";
  } else if (storing) {
    processor.saveRegister(i);
    status = "Stored register " + juce::String(i + 1);
  } else
    status =
        processor.recallRegister(i) ? "Register recalled" : "Empty register";
}
void Lexicon224Editor::timerCallback() {
  if (revision != processor.presetRevision.load()) {
    revision = processor.presetRevision.load();
    armPickup();
  }
  for (size_t i = 0; i < 6; ++i) {
    if (active[i] && !dragging[i])
      panel.sliders[i].setValue(
          processor.state.getRawParameterValue(ids[i])->load(),
          juce::dontSendNotification);
    panel.displayButtons[i].setToggleState(active[i],
                                           juce::dontSendNotification);
  }
  for (int i = 0; i < 8; ++i)
    programs.buttons[size_t(i)].setToggleState(
        i < 6 ? processor.getCurrentProgram() == i
              : processor.state
                        .getRawParameterValue(i == 6 ? "mode" : "optimize")
                        ->load() > .5f,
        juce::dontSendNotification);
  modes.buttons[1].setToggleState(storing, juce::dontSendNotification);
  modes.buttons[2].setToggleState(call, juce::dontSendNotification);
  modes.buttons[3].setToggleState(shift, juce::dontSendNotification);
  float value = held >= 0
                    ? processor.state.getRawParameterValue(ids[held])->load()
                    : (processor.state.getRawParameterValue("bass")->load() +
                       processor.state.getRawParameterValue("mid")->load()) *
                          .5f;
  display.unit = held < 2 ? "sec" : held == 5 ? "ms" : held == 4 ? "" : "Hz";
  if ((held == 2 || held == 3) && value >= 1000) {
    value /= 1000;
    display.unit = "kHz";
  }
  display.value = juce::String(
      value, (display.unit == "sec" || display.unit == "kHz") ? 1 : 0);
  display.overflow = processor.overflow.load();
  display.repaint();
  meter.left = processor.meters[0].load();
  meter.right = processor.meters[1].load();
  meter.repaint();
  repaint();
}
void Lexicon224Editor::paint(juce::Graphics &g) {
  auto bounds = getLocalBounds().toFloat();
  g.fillAll(juce::Colour(0xff1c1c1c));
  g.setGradientFill(juce::ColourGradient(juce::Colour(0xff252525), 0, 0,
                                         juce::Colour(0xff252525), 0,
                                         bounds.getHeight(), false));
  juce::ColourGradient metal(juce::Colour(0xff252525), 0, 0,
                             juce::Colour(0xff252525), 0, bounds.getHeight(),
                             false);
  metal.addColour(.5, juce::Colour(0xff1a1a1a));
  g.setGradientFill(metal);
  g.fillRoundedRectangle(bounds, 6);
  g.setColour(juce::Colour(0x14777777));
  for (int y = 1; y < getHeight(); y += 3)
    g.drawHorizontalLine(y, 1, float(getWidth() - 1));
  auto face = bounds.reduced(14);
  g.setGradientFill(juce::ColourGradient(
      juce::Colour(0xffeee6c5), 14, 0, juce::Colour(0xffc7bea2),
      float(getWidth() - 14), float(getHeight()), false));
  g.fillRect(face);
  g.setColour(juce::Colour(0xff8c8571));
  g.drawRect(face, 2);
  // 8 px rack screws, six-pixel inset from each outer edge.
  for (float x : {6.f, float(getWidth() - 14)})
    for (float y : {6.f, float(getHeight() - 14)}) {
      g.setColour(juce::Colour(0xff0f0f0f));
      g.fillEllipse(x, y, 8, 8);
      g.setColour(juce::Colour(0xff333333));
      g.drawEllipse(x, y, 8, 8, 1);
      g.drawLine(x + 2, y + 4, x + 6, y + 4, 1);
    }
  g.setColour(juce::Colour(0xff302f29));
  g.setFont(juce::Font(39, juce::Font::bold));
  g.drawText("lexicon 224", 47, 37, 250, 52, juce::Justification::centredLeft);
  g.fillRect(48, 87, 239, 4);
  g.setFont(juce::Font("Serif", 27, juce::Font::italic));
  g.drawText("Digital Reverb", 293, 43, 207, 43, juce::Justification::centred);
  g.setFont(12);
  g.drawText("HEADROOM · dB", 64, 183, 180, 20, juce::Justification::centred);
  g.drawText("PROGRAM / SUBPROGRAM", 66, 208, 400, 20,
             juce::Justification::centred);
  g.drawText("REVERB TIME", 66, 704, 125, 20, juce::Justification::centred);
  g.drawText("REGISTERS / EXTENDED BANKS", 66, 750, 400, 20,
             juce::Justification::centred);
  g.drawText("DRY / WET", 51, 845, 90, 20, juce::Justification::centredLeft);
  g.drawText("MODE RATE", 51, 875, 90, 20, juce::Justification::centredLeft);
  g.drawText("DECAY OPT", 290, 875, 90, 20, juce::Justification::centredLeft);
  g.setFont(11);
  g.drawText(status, 34, 924, 467, 20, juce::Justification::centred);
}
void Lexicon224Editor::resized() {
  presetButton.setBounds(402, 91, 90, 24);
  meter.setBounds(66, 119, 191, 64);
  display.setBounds(257, 119, 210, 64);
  for (int i = 0; i < 8; ++i)
    programs.buttons[size_t(i)].setBounds(65 + (i % 4) * 101,
                                          234 + (i / 4) * 66, 100, 62);
  for (int i = 0; i < 5; ++i)
    modes.buttons[size_t(i)].setBounds(65 + i * 81, 379, 80, 59);
  for (int i = 0; i < 6; ++i) {
    panel.sliders[size_t(i)].setBounds(61 + i * 68, 465, 66, 210);
    panel.displayButtons[size_t(i)].setBounds(63 + i * 68, 678, 65, 39);
  }
  for (int i = 0; i < 4; ++i)
    registers.buttons[size_t(i)].setBounds(65 + i * 101, 775, 100, 51);
  mix.setBounds(143, 844, 205, 24);
  mono.setBounds(358, 844, 130, 24);
  rate.setBounds(137, 874, 143, 24);
  amount.setBounds(375, 874, 115, 24);
  converter.setBounds(65, 902, 402, 22);
}
