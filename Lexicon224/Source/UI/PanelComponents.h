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
    g.fillEllipse(x - 7.2f, y - 7.2f, 14.4f, 14.4f);
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
    g.drawEllipse(x - 6, y - 6, 12, 12, .8f);
    g.setColour(juce::Colour(on ? 0x90ffb7a3 : 0x557c6669));
    g.fillEllipse(x - 2.7f, y - 3.6f, 3.2f, 1.8f);
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
    g.setColour(juce::Colour(0xff080909));
    g.fillRect(r);
    auto body = r.reduced(2);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff505156), 0, 2,
                                           juce::Colour(0xff28292d), 0,
                                           r.getBottom(), false));
    g.fillRoundedRectangle(body, 1.2f);
    g.setColour(juce::Colour(0xff77777b).withAlpha(.55f));
    g.drawLine(body.getX(), body.getY(), body.getRight(), body.getY(), .8f);
    g.setColour(juce::Colour(0xff191a1c));
    g.drawLine(body.getRight(), body.getY(), body.getRight(), body.getBottom(),
               1.5f);
    led(g, r.getCentreX(), 12, b.getToggleState());
    auto key = body.withTrimmedTop(27).withTrimmedBottom(5).reduced(1, 0);
    if (down)
      key.translate(0, 1.5f);
    juce::Path silhouette;
    silhouette.addRoundedRectangle(key, 1.2f);
    juce::DropShadow(juce::Colour(0xb0000000), 3, {2, 3})
        .drawForPath(g, silhouette);
    g.setColour(juce::Colour(0xff17181c));
    g.fillRoundedRectangle(key, 1.2f);
    auto top = key.reduced(1.4f).withTrimmedBottom(2);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff41424a), 0,
                                           top.getY(), juce::Colour(0xff303139),
                                           0, top.getBottom(), false));
    g.fillRoundedRectangle(top, .8f);
    g.setColour(juce::Colour(0xff85858a).withAlpha(.75f));
    g.drawLine(key.getX() + 1, key.getY() + 1, key.getRight() - 1,
               key.getY() + 1, 1.f);
    g.setColour(juce::Colour(0xff676871));
    g.drawLine(key.getX() + 1, key.getY() + 1, key.getX() + 1,
               key.getBottom() - 2, .8f);
    g.setColour(juce::Colour(0xff101115));
    g.drawLine(key.getX() + 1, key.getBottom() - 1, key.getRight(),
               key.getBottom() - 1, 2.f);
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
    juce::Path silhouette;
    silhouette.addRoundedRectangle(cap, 1.4f);
    juce::DropShadow(juce::Colour(0x850c0b08), 5, {3, 5})
        .drawForPath(g, silhouette);
    g.setColour(juce::Colour(0xff222329));
    g.fillRoundedRectangle(cap, 1.4f);
    auto top = cap.reduced(1.5f).withTrimmedBottom(1);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff41434c), 0,
                                           top.getY(), juce::Colour(0xff30323a),
                                           0, top.getBottom(), false));
    g.fillRoundedRectangle(top, .6f);
    g.setColour(juce::Colour(0xff96979a));
    g.drawLine(cap.getX() + 1, cap.getY() + 1, cap.getRight() - 1,
               cap.getY() + 1, .9f);
    g.setColour(juce::Colour(0xff767982));
    g.drawLine(cap.getX() + 1, cap.getY() + 1, cap.getX() + 1,
               cap.getBottom() - 2, 1.2f);
    g.setColour(juce::Colour(0xff13151b));
    g.drawLine(cap.getX() + 1, cap.getBottom() - 1, cap.getRight() - 1,
               cap.getBottom() - 1, 1.8f);
    g.setColour(juce::Colour(0xff1c1d23));
    g.drawLine(cap.getRight() - 1, cap.getY() + 2, cap.getRight() - 1,
               cap.getBottom() - 1, 1.3f);
  }
};
namespace PanelGlass {
inline void paint(juce::Graphics &g, int width, int height, int offset = 0) {
  juce::ColourGradient glass(juce::Colour(0xff350708), 0, 0,
                             juce::Colour(0xff260204), 0, float(height), false);
  glass.addColour(.45, juce::Colour(0xff450809));
  g.setGradientFill(glass);
  g.fillAll();
  juce::Random grain(224);
  for (int i = 0; i < 1800; ++i) {
    float x = grain.nextFloat() * 396 - offset;
    float y = grain.nextFloat() * height;
    g.setColour(juce::Colour(0xffa56b66).withAlpha(grain.nextFloat() * .055f));
    g.fillRect(x, y, .8f, .8f);
  }
  g.setColour(juce::Colour(0x18c77c77));
  g.drawLine(0, .5f, float(width), .5f, 1);
  g.setColour(juce::Colour(0x60090001));
  g.fillRect(0, height - 2, width, 2);
}
} // namespace PanelGlass
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
    juce::Graphics::ScopedSaveState saved(g);
    g.addTransform(juce::AffineTransform(1, -.12f, 6.5f, 0, 1, 0));
    for (size_t i = 0; i < 7; ++i) {
      bool on = n >= 0 && (mask[n] & (1 << i));
      auto r = segments[i].translated(x, y);
      if (on) {
        g.setColour(juce::Colour(0x35ff2010));
        g.fillRoundedRectangle(r.expanded(2), 2);
      }
      g.setColour(juce::Colour(on ? 0xffe3231b : 0xff420909));
      g.fillRoundedRectangle(r, 1);
    }
  }

public:
  juce::String value = "2.8", unit = "sec";
  bool overflow = false;
  void paint(juce::Graphics &g) override {
    PanelGlass::paint(g, getWidth(), getHeight(), 215);
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
    PanelGlass::paint(g, getWidth(), getHeight());
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
