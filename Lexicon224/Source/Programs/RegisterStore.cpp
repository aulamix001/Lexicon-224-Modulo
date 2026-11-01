#include "RegisterStore.h"
void RegisterStore::store(int slot, const juce::ValueTree &s) {
  if (juce::isPositiveAndBelow(slot, 36))
    slots[size_t(slot)] = s.createCopy();
}
juce::ValueTree RegisterStore::recall(int slot) const {
  return juce::isPositiveAndBelow(slot, 36) ? slots[size_t(slot)].createCopy()
                                            : juce::ValueTree{};
}
juce::ValueTree RegisterStore::serialize() const {
  juce::ValueTree root("Registers");
  for (int i = 0; i < 36; ++i)
    if (slots[size_t(i)].isValid()) {
      juce::ValueTree entry("Slot");
      entry.setProperty("index", i, nullptr);
      entry.addChild(slots[size_t(i)].createCopy(), -1, nullptr);
      root.addChild(entry, -1, nullptr);
    }
  return root;
}
void RegisterStore::restore(const juce::ValueTree &root) {
  slots = {};
  for (auto child : root) {
    int i = child.getProperty("index", -1);
    if (juce::isPositiveAndBelow(i, 36) && child.getNumChildren() == 1)
      slots[size_t(i)] = child.getChild(0).createCopy();
  }
}
