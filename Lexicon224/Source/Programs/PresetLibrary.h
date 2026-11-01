#pragma once
#include "../PluginProcessor.h"
#include <JuceHeader.h>
class PresetLibrary {
public:
  struct Entry {
    juce::String id, name;
    juce::StringArray tags;
    juce::var data;
    bool user = false;
  };
  explicit PresetLibrary(juce::File storage = {});
  const std::vector<Entry> &entries() const { return items; }
  juce::Array<int> filter(const juce::String &, const juce::String &,
                          bool) const;
  bool favourite(const juce::String &) const;
  bool toggleFavourite(const juce::String &);
  juce::String apply(int, Lexicon224Processor &);
  bool save(const juce::String &, Lexicon224Processor &,
            const juce::String &tag);
  bool rename(int, const juce::String &);
  bool remove(int);

private:
  bool persist();
  std::vector<Entry> items;
  juce::StringArray favourites;
  juce::File file;
};
