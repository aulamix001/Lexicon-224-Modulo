#include "PluginEditor.h"
Lexicon224Editor::Lexicon224Editor(Lexicon224Processor &p)
    : AudioProcessorEditor(&p), processor(p) {
  setLookAndFeel(&look);
  addAndMakeVisible(surface);
  surface.addAndMakeVisible(meter);
  surface.addAndMakeVisible(display);
  open.getProperties().set("open", true);
  open.setTooltip("Open preset browser");
  surface.addAndMakeVisible(open);
  open.onClick = [this] {
    if (!window)
      window = std::make_unique<PresetWindow>(processor);
    if (window->isVisible())
      window->setVisible(false);
    else
      window->showBeside(*this);
  };
  for (int i = 0; i < 8; ++i) {
    auto &b = programs[size_t(i)];
    surface.addAndMakeVisible(b);
    b.setTooltip(i < 6 ? processor.getProgramName(i)
                       : "Algorithm not implemented in the current DSP");
    b.onClick = [this, i] {
      if (i < 6)
        processor.setCurrentProgram(i);
    };
  }
  for (int i = 0; i < 8; ++i) {
    auto &b = functions[size_t(i)];
    surface.addAndMakeVisible(b);
    b.onClick = [this, i] {
      if (i == 0) {
        immed = !immed;
      }
      if (i == 1 || i == 2) {
        juce::PopupMenu menu;
        menu.addItem(1, "This hardware feature is not implemented", false);
        menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(
            &functions[size_t(i)]));
      }
      if (i == 3 || i == 4) {
        auto id = i == 3 ? "mode" : "optimize";
        set(id, processor.state.getRawParameterValue(id)->load() > .5f ? 0 : 1);
      }
      if (i == 5 || i == 6)
        adjust("mix", i == 5 ? -5.f : 5.f);
      if (i == 7)
        set("mix", processor.state.getRawParameterValue("mix")->load() > 99
                       ? 50
                       : 100);
    };
  }
  const char *tips[]{"Immediate fader control",
                     "System noise (unavailable)",
                     "Rear outputs (configure in host)",
                     "Mode enhancement",
                     "Decay optimization",
                     "Decrease wet mix",
                     "Increase wet mix",
                     "Solo wet return"};
  for (int i = 0; i < 8; ++i)
    functions[size_t(i)].setTooltip(tips[i]);
  const double defaults[]{3, 2.6, 540, 4400, 25, 24};
  const double centreY[]{562, 577, 622, 532, 598, 647};
  for (size_t i = 0; i < 6; ++i) {
    auto &s = sliders[i];
    auto *parameter = p.state.getParameter(ids[i]);
    auto range = parameter->getNormalisableRange();
    double knot = parameter->convertTo0to1(float(defaults[i])),
           visual = (665 - centreY[i]) / 195;
    s.setNormalisableRange(juce::NormalisableRange<double>(
        range.start, range.end,
        [range, knot, visual](double, double, double t) {
          double n = t <= visual
                         ? t * knot / visual
                         : knot + (t - visual) * (1 - knot) / (1 - visual);
          return double(range.convertFrom0to1(float(n)));
        },
        [range, knot, visual](double, double, double v) {
          double n = range.convertTo0to1(float(v));
          return n <= knot ? n * visual / knot
                           : visual + (n - knot) * (1 - visual) / (1 - knot);
        },
        [range](double, double, double v) {
          return double(range.snapToLegalValue(float(v)));
        }));
    s.setSliderStyle(juce::Slider::LinearVertical);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setValue(p.state.getRawParameterValue(ids[i])->load(),
               juce::dontSendNotification);
    surface.addAndMakeVisible(s);
    s.setTooltip(ids[i]);
    s.onDragStart = [this, i] {
      held = int(i);
      processor.state.getParameter(ids[i])->beginChangeGesture();
    };
    s.onDragEnd = [this, i] {
      held = -1;
      processor.state.getParameter(ids[i])->endChangeGesture();
    };
    s.onValueChange = [this, i] {
      auto *target = processor.state.getParameter(ids[i]);
      target->setValueNotifyingHost(
          target->convertTo0to1(float(sliders[i].getValue())));
    };
  }
  for (int i = 0; i < 6; ++i) {
    surface.addAndMakeVisible(lower[size_t(i)]);
    lower[size_t(i)].onClick = [this, i] {
      adjust(i < 2   ? "diffusion"
             : i < 4 ? "modeRate"
                     : "decayAmount",
             (i % 2 ? 1.f : -1.f) * (i < 2 ? .05f : 1.f));
    };
  }
  setResizable(true, false);
  getConstrainer()->setFixedAspectRatio(506.0 / 865.0);
  setResizeLimits(405, 692, 759, 1298);
  setSize(506, 865);
  timerCallback();
  startTimerHz(30);
}
Lexicon224Editor::~Lexicon224Editor() {
  stopTimer();
  window.reset();
  setLookAndFeel(nullptr);
}
void Lexicon224Editor::set(const char *id, float v) {
  auto *p = processor.state.getParameter(id);
  p->beginChangeGesture();
  p->setValueNotifyingHost(p->convertTo0to1(v));
  p->endChangeGesture();
}
void Lexicon224Editor::adjust(const char *id, float delta) {
  auto *p = processor.state.getParameter(id);
  set(id, juce::jlimit(
              p->getNormalisableRange().start, p->getNormalisableRange().end,
              processor.state.getRawParameterValue(id)->load() + delta));
}
void Lexicon224Editor::timerCallback() {
  for (size_t i = 0; i < 6; ++i)
    if (!sliders[i].isMouseButtonDown())
      sliders[i].setValue(processor.state.getRawParameterValue(ids[i])->load(),
                          juce::dontSendNotification);
  for (int i = 0; i < 8; ++i)
    programs[size_t(i)].setToggleState(i == processor.getCurrentProgram(),
                                       juce::dontSendNotification);
  functions[0].setToggleState(immed, juce::dontSendNotification);
  functions[3].setToggleState(
      processor.state.getRawParameterValue("mode")->load() > .5f,
      juce::dontSendNotification);
  functions[4].setToggleState(
      processor.state.getRawParameterValue("optimize")->load() > .5f,
      juce::dontSendNotification);
  functions[7].setToggleState(
      processor.state.getRawParameterValue("mix")->load() > 99,
      juce::dontSendNotification);
  float v = held >= 0 ? processor.state.getRawParameterValue(ids[held])->load()
                      : (processor.state.getRawParameterValue("bass")->load() +
                         processor.state.getRawParameterValue("mid")->load()) *
                            .5f;
  display.unit = held < 2 ? "sec" : held == 5 ? "ms" : held == 4 ? "" : "Hz";
  if ((held == 2 || held == 3) && v >= 1000) {
    v /= 1000;
    display.unit = "kHz";
  }
  display.value =
      juce::String(v, display.unit == "sec" || display.unit == "kHz" ? 1 : 0);
  display.overflow = processor.overflow.load();
  meter.left = processor.meters[0].load();
  meter.right = processor.meters[1].load();
  meter.overflow = display.overflow;
  display.repaint();
  meter.repaint();
}
void Lexicon224Editor::paint(juce::Graphics &graphics) {
  juce::Graphics::ScopedSaveState saved(graphics);
  auto &g = graphics;
  float scale = juce::jmin(getWidth() / 506.f, getHeight() / 865.f);
  g.addTransform(juce::AffineTransform::scale(scale));
  g.fillAll(juce::Colour(0xff111212));
  auto face = juce::Rectangle<float>(13, 37, 480, 808);
  juce::ColourGradient paper(juce::Colour(0xffe9e1c9), 30, 60,
                             juce::Colour(0xffddd3b5), 480, 820, false);
  paper.addColour(.5, juce::Colour(0xffe9e1c9));
  g.setGradientFill(paper);
  g.fillRoundedRectangle(face, 3);
  juce::Random rng(224);
  for (int i = 0; i < 11000; ++i) {
    float x = 14 + rng.nextFloat() * 478, y = 38 + rng.nextFloat() * 805;
    g.setColour(juce::Colour(0xff5e5840).withAlpha(rng.nextFloat() * .035f));
    g.fillRect(x, y, 1.f, 1.f);
  }
  for (int i = 0; i < 90; ++i) {
    float x = 20 + rng.nextFloat() * 466, y = 50 + rng.nextFloat() * 775;
    juce::ColourGradient wash(juce::Colour(0x075e5840), x, y,
                              juce::Colours::transparentBlack,
                              x + 25 + rng.nextFloat() * 50, y, true);
    g.setGradientFill(wash);
    g.fillEllipse(x - 45, y - 30, 90, 60);
  }
  for (int i = 0; i < 18; ++i) {
    float x = 22 + rng.nextFloat() * 460, y = 140 + rng.nextFloat() * 670;
    g.setColour(juce::Colour(0x106a624d));
    g.drawLine(x, y, x + 8 + rng.nextFloat() * 16,
               y - 12 - rng.nextFloat() * 14, .7f);
    g.setColour(juce::Colour(0x20fffbea));
    g.drawLine(x + 1, y, x + 9 + rng.nextFloat() * 16,
               y - 12 - rng.nextFloat() * 14, .6f);
  }
  for (int i = 0; i < 11; ++i) {
    g.setColour(juce::Colour(0xff25251e).withAlpha(.025f));
    g.drawRoundedRectangle(face.reduced(float(i)), 3, 1);
  }
  for (int x : {5, 496}) {
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff414244), float(x),
                                           0, juce::Colour(0xff171819),
                                           float(x + 6), 0, false));
    g.fillRect(x, 12, 6, 842);
  }
  g.setColour(juce::Colour(0xff797a78));
  g.fillRoundedRectangle(287, -8, 186, 34, 3);
  g.setColour(juce::Colour(0xff555654));
  g.fillRect(311, 0, 138, 18);
  g.setColour(juce::Colour(0xffc6c6bf));
  g.fillRect(307, 23, 152, 14);
  for (int i = 0; i < 90; ++i) {
    float x = 20 + rng.nextFloat() * 466, y = 50 + rng.nextFloat() * 775;
    juce::ColourGradient wash(juce::Colour(0x075e5840), x, y,
                              juce::Colours::transparentBlack,
                              x + 25 + rng.nextFloat() * 50, y, true);
    g.setGradientFill(wash);
    g.fillEllipse(x - 45, y - 30, 90, 60);
  }
  for (int i = 0; i < 18; ++i) {
    float x = 22 + rng.nextFloat() * 460, y = 140 + rng.nextFloat() * 670;
    g.setColour(juce::Colour(0x106a624d));
    g.drawLine(x, y, x + 8 + rng.nextFloat() * 16,
               y - 12 - rng.nextFloat() * 14, .7f);
    g.setColour(juce::Colour(0x20fffbea));
    g.drawLine(x + 1, y, x + 9 + rng.nextFloat() * 16,
               y - 12 - rng.nextFloat() * 14, .6f);
  }
  for (int i = 0; i < 11; ++i) {
    g.setColour(juce::Colour(0xff828380));
    g.drawLine(319.f + i * 12, 25, 317.f + i * 12, 36, 2);
  }
  auto label = [&](juce::String text, int x, int y, int w, int h,
                   float size = 11) {
    g.setColour(juce::Colour(0xff33332b));
    g.setFont(PanelFonts::regular(size * 1.18f));
    g.drawFittedText(text, x, y, w, h, juce::Justification::centred, 2, .7f);
  };
  g.setColour(juce::Colour(0xff282822));
  g.setFont(PanelFonts::bold(39));
  g.drawFittedText("lexicon 224", 52, 58, 195, 42,
                   juce::Justification::centredLeft, 1, .8f);
  g.fillRect(53, 98, 192, 4);
  g.setFont(PanelFonts::italic(31));
  g.drawFittedText("Digital Reverb", 266, 59, 189, 43,
                   juce::Justification::centredRight, 1, .8f);
  g.setColour(juce::Colour(0xff1a0807));
  g.fillRect(60, 119, 396, 72);
  label("HEADROOM-dB", 65, 193, 160, 16, 10);
  label("PROGRAM", 204, 208, 105, 20, 13);
  label("+CLK=CHORUS", 324, 193, 128, 16, 10);
  g.setColour(juce::Colour(0xff575347));
  g.drawLine(335, 202, 335, 195, 1);
  g.drawLine(332, 198, 335, 195, 1);
  g.drawLine(338, 198, 335, 195, 1);
  const char *names[]{"Sm Hall B", "VocPlate",  "Lg Hall B", "Chamber",
                      "PercPlate", "Sm Hall A", "Room A",    "Const Plate"};
  for (int i = 0; i < 8; ++i) {
    int x = 65 + i * 49;
    label(juce::String(i + 1), x, 229, 49, 13, 11);
    label(names[i], x - 1, 241, 51, 13, 10);
  }
  const char *fn[]{"IMMED",      "SYS\nNOISE", "REAR\nOUTS", "MODE\nENH",
                   "DECAY\nOPT", "",           "",           "SOLO"};
  for (int i = 0; i < 8; ++i)
    label(fn[i], 63 + i * 49, 421, 53, 30, 11);
  label("< DRY/WET >", 304, 421, 99, 15, 11);
  label("MIX", 325, 439, 55, 14, 10);
  g.setColour(juce::Colour(0xff777161));
  g.drawLine(305, 446, 325, 446, 1);
  g.drawLine(380, 446, 403, 446, 1);
  const char *faders[]{"BASS",          "MID",   "CROSS\nOVER",
                       "TREBLE\nDECAY", "DEPTH", "PRE-\nDELAY"};
  for (int i = 0; i < 6; ++i)
    label(faders[i], 60 + i * 67, 678, 60, 30, 12);
  label("REVERB TIME", 68, 699, 116, 17, 11);
  g.setColour(juce::Colour(0xff777161));
  g.drawLine(65, 708, 77, 708, 1);
  g.drawLine(179, 708, 194, 708, 1);
  const char *bottom[]{"<    REVERB    >\nDIFFUSION",
                       "<     MODE     >\nENHANCEMENT",
                       "<    DECAY    >\nOPTIMIZATION"};
  for (int i = 0; i < 3; ++i)
    label(bottom[i], 65 + i * 131, 786, 131, 34, 11);
}
void Lexicon224Editor::resized() {
  float scale = juce::jmin(getWidth() / 506.f, getHeight() / 865.f);
  surface.setBounds(0, 0, 506, 865);
  surface.setTransform(juce::AffineTransform::scale(scale));
  meter.setBounds(62, 121, 215, 68);
  display.setBounds(277, 121, 177, 68);
  open.setBounds(458, 120, 32, 33);
  for (int i = 0; i < 8; ++i) {
    programs[size_t(i)].setBounds(65 + i * 49, 254, 49, 68);
    functions[size_t(i)].setBounds(65 + i * 49, 350, 49, 68);
  }
  for (int i = 0; i < 6; ++i) {
    sliders[size_t(i)].setBounds(57 + i * 67, 455, 66, 225);
    lower[size_t(i)].setBounds(65 + i * 65, 715, 65, 65);
  }
}
