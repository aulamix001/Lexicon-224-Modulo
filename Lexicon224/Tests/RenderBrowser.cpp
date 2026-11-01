#include "PluginEditor.h"
#include "UI/PresetBrowser.h"
#include <iostream>
int main(int argc, char **argv) {
  juce::ScopedJuceInitialiser_GUI gui;
  Lexicon224Processor processor;
  Lexicon224Editor browser(processor);
  // Capture the steady-state panel without JUCE's transient startup splash.
  // The actual plugin retains the framework's startup branding behavior.
  for (auto *child : browser.getChildren())
    if (dynamic_cast<juce::JUCESplashScreen *>(child))
      child->setVisible(false);
  int keys = 0, faders = 0, lit = 0, programRow = 0, functionRow = 0,
      lowerRow = 0;
  bool positions = true;
  const double expectedY[]{562, 577, 622, 532, 598, 647};
  std::function<void(juce::Component &)> inspect =
      [&](juce::Component &component) {
        if (auto *button = dynamic_cast<juce::TextButton *>(&component)) {
          ++keys;
          if (button->getY() == 254)
            ++programRow;
          if (button->getY() == 350)
            ++functionRow;
          if (button->getY() == 715)
            ++lowerRow;
          if (button->getToggleState())
            ++lit;
        }
        if (auto *slider = dynamic_cast<juce::Slider *>(&component)) {
          double centre =
              slider->getY() + 15 +
              195 * (1 - slider->valueToProportionOfLength(slider->getValue()));
          if (faders >= 6 || std::abs(centre - expectedY[faders]) > 1)
            positions = false;
          ++faders;
        }
        for (auto *child : component.getChildren())
          inspect(*child);
      };
  inspect(browser);
  if (!positions || programRow != 8 || functionRow != 8 || lowerRow != 6 ||
      keys != 23 || faders != 6 || lit != 4 ||
      processor.getCurrentAlgorithm() != 2) {
    std::cerr << "Panel structure/state mismatch";
    return 2;
  }
#if JUCE_WINDOWS
  // Verify an actual independent native preset window, not only a static
  // render.
  juce::TextButton *opener = nullptr;
  std::function<void(juce::Component &)> findOpen = [&](juce::Component &c) {
    if (auto *b = dynamic_cast<juce::TextButton *>(&c))
      if (b->getProperties().contains("open"))
        opener = b;
    for (auto *child : c.getChildren())
      findOpen(*child);
  };
  findOpen(browser);
  if (!opener)
    return 3;
  opener->onClick();
  PresetWindow *floating = nullptr;
  for (int i = 0; i < juce::Desktop::getInstance().getNumComponents(); ++i)
    if (auto *w = dynamic_cast<PresetWindow *>(
            juce::Desktop::getInstance().getComponent(i)))
      floating = w;
  if (!floating || !floating->isVisible() || !floating->getPeer())
    return 4;
  opener->onClick();
  if (floating->isVisible())
    return 5;
#endif
  PresetBrowser organizer(processor);
  auto image =
      argc > 2 ? organizer.createComponentSnapshot(organizer.getLocalBounds())
               : browser.createComponentSnapshot(browser.getLocalBounds());
  juce::File output = juce::File::getCurrentWorkingDirectory().getChildFile(
      argc > 1 ? argv[1] : "preset-browser.png");
  juce::FileOutputStream stream(output);
  juce::PNGImageFormat format;
  if (!stream.openedOk())
    return 1;
  stream.setPosition(0);
  if (stream.truncate().failed() || !format.writeImageToStream(image, stream))
    return 1;
  std::cout << output.getFullPathName() << '\n';
}
