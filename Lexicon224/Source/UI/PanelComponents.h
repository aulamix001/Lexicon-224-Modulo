#pragma once
#include <JuceHeader.h>
class PanelLook : public juce::LookAndFeel_V4 {
public:
  PanelLook() {
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff303238));
    setColour(juce::TextButton::textColourOffId, juce::Colour(0xffece5c8));
    setColour(juce::TextButton::textColourOnId, juce::Colours::white);
  }
  void drawButtonBackground(juce::Graphics &g, juce::Button &b,
                            const juce::Colour &, bool hover,
                            bool down) override {
    auto r = b.getLocalBounds().toFloat().reduced(1);
    g.setColour(juce::Colour(0xff101112));
    g.fillRect(r);
    r.reduce(3, 3);
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(down ? 0xff24262a : 0xff4b4d50), 0, r.getY(),
        juce::Colour(0xff27292e), 0, r.getBottom(), false));
    g.fillRect(r);
    g.setColour(juce::Colour(0xff77787a));
    g.drawHorizontalLine(int(r.getY()), r.getX(), r.getRight());
    if (b.getToggleState()) {
      g.setColour(juce::Colour(0x55ff2018));
      g.fillEllipse(r.getCentreX() - 10, r.getY() + 1, 20, 15);
      g.setColour(juce::Colour(0xffff3328));
      g.fillEllipse(r.getCentreX() - 4, r.getY() + 4, 8, 8);
    } else {
      g.setColour(juce::Colour(hover ? 0xff702524 : 0xff38191c));
      g.fillEllipse(r.getCentreX() - 4, r.getY() + 4, 8, 8);
    }
  }
  void drawLinearSlider(juce::Graphics &g, int x, int y, int w, int h,
                        float pos, float, float, juce::Slider::SliderStyle,
                        juce::Slider &) override {
    float cx = float(x + w / 2);
    g.setColour(juce::Colour(0xff161712));
    g.fillRoundedRectangle(cx - 5, float(y), 10, float(h), 5);
    g.setColour(juce::Colour(0xff77715c));
    for (int i = 0; i < 6; ++i) {
      float yy = float(y) + float(h * i) / 5;
      g.drawLine(cx + 12, yy, cx + 31, yy, 1);
    }
    juce::Rectangle<float> cap(cx - 23, pos - 11, 46, 23);
    g.setColour(juce::Colour(0x55202018));
    g.fillRect(cap.translated(4, 5));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff66676b), 0,
                                           pos - 11, juce::Colour(0xff26282d),
                                           0, pos + 11, false));
    g.fillRect(cap);
    g.setColour(juce::Colour(0xff18191c));
    g.drawRect(cap, 2);
  }
};
class NumerDisplay : public juce::Component {
public:
  juce::String value = "2.5", unit = "sec";
  bool overflow = false;
  void paint(juce::Graphics &g) override {
    auto r = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff200203), 0, 0,
                                           juce::Colour(0xff490507), 0,
                                           r.getHeight(), false));
    g.fillRect(r);
    g.setColour(juce::Colour(0xffac1915));
    g.setFont(juce::Font(48, juce::Font::italic));
    g.drawText(overflow ? "E" + value : value,
               r.reduced(4).withTrimmedRight(30),
               juce::Justification::centredRight);
    g.setFont(12);
    for (int i = 0; i < 4; ++i) {
      auto text = juce::StringArray{"sec", "ms", "Hz", "kHz"}[i];
      g.setColour(text == unit ? juce::Colour(0xffff3c30)
                               : juce::Colour(0xff8c5b56));
      g.drawText(text, getWidth() - 30, 3 + i * 15, 29, 15,
                 juce::Justification::centred);
    }
  }
};
class HeadroomMeter : public juce::Component {
public:
  float left = 0, right = 0;
  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colour(0xff290506));
    g.setFont(12);
    for (int row = 0; row < 2; ++row) {
      g.setColour(juce::Colour(0xffd4b0a5));
      g.drawText(row ? "R" : "L", 4, 12 + row * 25, 15, 18,
                 juce::Justification::centred);
      for (int i = 0; i < 5; ++i) {
        float threshold = std::pow(10.f, float(-24 + i * 6) / 20);
        bool lit = (row ? right : left) >= threshold;
        g.setColour(juce::Colour(lit ? 0xfffa221a : 0xff4d1212));
        g.fillEllipse(float(28 + i * 29), float(15 + row * 25), 10, 10);
        if (row == 0) {
          g.setColour(juce::Colour(0xffd4b0a5));
          g.drawText(juce::String(24 - i * 6), 22 + i * 29, 28, 24, 15,
                     juce::Justification::centred);
        }
      }
    }
  }
};
struct ProgramButtons {
  std::array<juce::TextButton, 8> buttons;
};
struct ModeButtons {
  std::array<juce::TextButton, 5> buttons;
};
struct RegisterButtons {
  std::array<juce::TextButton, 4> buttons;
};
struct SliderPanel {
  std::array<juce::Slider, 6> sliders;
  std::array<juce::TextButton, 6> displayButtons;
};
