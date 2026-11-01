#pragma once
#include <JuceHeader.h>
#include <PresetData.h>
namespace PanelFonts {
inline juce::Font regular(float height) {
  static auto face = juce::Typeface::createSystemTypefaceFor(
      PresetData::DejaVuSansCondensed_ttf,
      PresetData::DejaVuSansCondensed_ttfSize);
  return juce::Font(face).withHeight(height).withHorizontalScale(.85f);
}
inline juce::Font bold(float height) {
  static auto face = juce::Typeface::createSystemTypefaceFor(
      PresetData::DejaVuSansCondensedBold_ttf,
      PresetData::DejaVuSansCondensedBold_ttfSize);
  return juce::Font(face).withHeight(height);
}
inline juce::Font italic(float height) {
  static auto face = juce::Typeface::createSystemTypefaceFor(
      PresetData::DejaVuSerifItalic_ttf, PresetData::DejaVuSerifItalic_ttfSize);
  return juce::Font(face).withHeight(height);
}
} // namespace PanelFonts
class PanelLook : public juce::LookAndFeel_V4 {
public:
  void drawButtonText(juce::Graphics &, juce::TextButton &, bool,
                      bool) override {}
  static void led(juce::Graphics &g, float x, float y, bool on) {
    g.setColour(juce::Colour(0xff100e0f));
    g.fillEllipse(x - 8, y - 8, 16, 16);
    if (on) {
      for (int r = 12; r >= 5; --r) {
        g.setColour(juce::Colour(0xffff2b1d).withAlpha(.025f));
        g.fillEllipse(x - r, y - r, float(r * 2), float(r * 2));
      }
    }
    juce::ColourGradient c(juce::Colour(on ? 0xffff6758 : 0xff734346), x - 2,
                           y - 3, juce::Colour(on ? 0xffff1c0b : 0xff280c10),
                           x + 4, y + 6, false);
    g.setGradientFill(c);
    g.fillEllipse(x - 5, y - 5, 10, 10);
    g.setColour(juce::Colour(0x558d7778));
    g.drawEllipse(x - 6, y - 6, 12, 12, 1);
  }
  void drawButtonBackground(juce::Graphics &g, juce::Button &b,
                            const juce::Colour &, bool, bool down) override {
    if (b.getProperties().contains("open")) {
      g.setColour(juce::Colour(0xff8b8978));
      auto x = b.getWidth() * .5f;
      g.drawLine(x - 10, 10, x, 1, 3);
      g.drawLine(x, 1, x + 10, 10, 3);
      g.setFont(10);
      g.drawText("OPEN", 0, 14, b.getWidth(), 16, juce::Justification::centred);
      return;
    }
    auto r = b.getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff0b0b0b));
    g.fillRect(r);
    auto body = r.reduced(3);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff44464a), 0, 3,
                                           juce::Colour(0xff24252a), 0,
                                           r.getBottom(), false));
    g.fillRect(body);
    led(g, r.getCentreX(), 12, b.getToggleState());
    auto key = body.withTrimmedTop(26).withTrimmedBottom(4);
    if (down)
      key.translate(0, 2);
    g.setColour(juce::Colour(0xff17191c));
    g.fillRect(key.translated(2, 3));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff404149), 0,
                                           key.getY(), juce::Colour(0xff303139),
                                           0, key.getBottom(), false));
    g.fillRect(key);
    g.setColour(juce::Colour(0xff65666b));
    g.drawLine(key.getX(), key.getY(), key.getRight(), key.getY());
    g.drawLine(key.getX(), key.getY(), key.getX(), key.getBottom());
  }
  void drawLinearSlider(juce::Graphics &g, int, int, int, int, float, float,
                        float, juce::Slider::SliderStyle,
                        juce::Slider &s) override {
    float x = s.getWidth() * .5f;
    g.setColour(juce::Colour(0xff777260));
    g.fillRoundedRectangle(x - 6, 14, 12, 197, 6);
    g.setColour(juce::Colour(0xff080a08));
    g.fillRoundedRectangle(x - 5, 15, 10, 195, 5);
    g.setColour(juce::Colour(0xff8a8578));
    for (int i = 0; i < 6; ++i) {
      float y = 15 + 39.f * i;
      g.drawLine(x + 12, y, x + 37, y, 1.4f);
    }
    float y = 15 + 195 * float(1 - s.valueToProportionOfLength(s.getValue()));
    juce::Rectangle<float> cap(x - 26, y - 13, 52, 26);
    for (int i = 5; i > 0; --i) {
      g.setColour(juce::Colour(0x14201d15));
      g.fillRect(cap.translated(float(i), float(i + 3)));
    }
    g.setColour(juce::Colour(0xff24252a));
    g.fillRect(cap);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff42444c), 0, y - 13,
                                           juce::Colour(0xff303138), 0, y + 13,
                                           false));
    g.fillRect(cap.reduced(2));
    g.setColour(juce::Colour(0xff808188));
    g.drawLine(cap.getX() + 1, cap.getY() + 1, cap.getRight() - 1,
               cap.getY() + 1, 1.3f);
    g.drawLine(cap.getX() + 1, cap.getY() + 1, cap.getX() + 1,
               cap.getBottom() - 1, 1);
  }
};
class NumerDisplay : public juce::Component {
  static void digit(juce::Graphics &g, int n, float x, float y) {
    static constexpr int mask[]{0x3f, 0x06, 0x5b, 0x4f, 0x66,
                                0x6d, 0x7d, 0x07, 0x7f, 0x6f};
    const std::array<juce::Rectangle<float>, 7> segments{{{5, 0, 20, 4},
                                                          {25, 4, 4, 16},
                                                          {23, 25, 4, 16},
                                                          {2, 42, 20, 4},
                                                          {0, 25, 4, 16},
                                                          {2, 4, 4, 16},
                                                          {4, 21, 20, 4}}};
    for (size_t i = 0; i < 7; ++i) {
      bool on = n >= 0 && (mask[n] & (1 << i));
      auto r = segments[i].translated(x, y);
      if (on) {
        g.setColour(juce::Colour(0x35ff2010));
        g.fillRoundedRectangle(r.expanded(2), 2);
      }
      g.setColour(juce::Colour(on ? 0xffe3231b : 0xff3b0808));
      g.fillRoundedRectangle(r, 1);
    }
  }

public:
  juce::String value = "2.8", unit = "sec";
  bool overflow = false;
  void paint(juce::Graphics &g) override {
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff280303), 0, 0,
                                           juce::Colour(0xff130203),
                                           float(getWidth()), 70, false));
    g.fillAll();
    auto text = value.removeCharacters(".");
    while (text.length() < 3)
      text = " " + text;
    float start = 13;
    for (int i = 0; i < 3; ++i)
      digit(g, text[i] >= '0' && text[i] <= '9' ? text[i] - '0' : -1,
            start + i * 33, 13);
    int decimal = value.indexOfChar('.');
    if (decimal >= 0) {
      int at = 3 - (value.length() - decimal - 1);
      g.setColour(juce::Colour(0xffe3231b));
      g.fillEllipse(start + at * 33 - 5, 56, 4, 4);
    }
    if (overflow) {
      g.setColour(juce::Colours::red);
      g.setFont(18);
      g.drawText("E", 0, 24, 13, 25, juce::Justification::centred);
    }
    g.setFont(juce::Font(11, juce::Font::italic));
    for (int i = 0; i < 4; ++i) {
      auto label = juce::StringArray{"sec", "ms", "Hz", "kHz"}[i];
      g.setColour(juce::Colour(0xffc2b4ac));
      g.drawText(label, 123, 5 + i * 14, 30, 15,
                 juce::Justification::centredLeft);
      g.setColour(juce::Colour(label == unit ? 0xffff2b1d : 0xff400b0b));
      g.fillEllipse(155, 8.f + i * 14, 9, 9);
    }
  }
};
class HeadroomMeter : public juce::Component {
public:
  float left = 0, right = 0;
  bool overflow = false;
  void paint(juce::Graphics &g) override {
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff250303), 0, 0,
                                           juce::Colour(0xff130203), 210, 0,
                                           false));
    g.fillAll();
    g.setFont(11);
    for (int row = 0; row < 2; ++row) {
      g.setColour(juce::Colour(0xffc4b5af));
      g.drawText(row ? "R" : "L", 8, 12 + row * 27, 12, 15,
                 juce::Justification::centred);
      for (int i = 0; i < 5; ++i) {
        bool on =
            (row ? right : left) >= std::pow(10.f, float(-24 + i * 6) / 20);
        g.setColour(juce::Colour(on ? 0xffff2b1d : 0xff3e0809));
        g.fillEllipse(31.f + i * 30, 12.f + row * 27, 8, 8);
        if (row == 0) {
          g.setColour(juce::Colour(0xffc4b5af));
          g.drawText(i == 4 ? "0dB" : juce::String(24 - i * 6), 26 + i * 30, 24,
                     29, 14, juce::Justification::centred);
        }
      }
    }
    g.setColour(juce::Colour(overflow ? 0xffff2b1d : 0xffbaa9a3));
    g.drawText("ovfl", 171, 49, 27, 14, juce::Justification::centred);
  }
};
