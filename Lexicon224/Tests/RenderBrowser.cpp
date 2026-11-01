#include "UI/PresetBrowser.h"
#include <iostream>
int main(int argc, char **argv) {
  juce::ScopedJuceInitialiser_GUI gui;
  Lexicon224Processor processor;
  PresetBrowser browser(processor);
  auto image = browser.createComponentSnapshot(browser.getLocalBounds());
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
