#include "FactoryPresets.h"
#include <PresetData.h>
const juce::Array<juce::var> &photoPresets() {
  static const auto rows = [] {
    juce::Array<juce::var> out;
    auto data = juce::JSON::parse(juce::String::fromUTF8(
        PresetData::catalog_json, PresetData::catalog_jsonSize));
    if (auto *list = data["presets"].getArray())
      for (auto row : *list)
        if (row["name"].isString())
          out.add(row);
    std::sort(out.begin(), out.end(), [](const auto &a, const auto &b) {
      return a["name"].toString().compareNatural(b["name"].toString()) < 0;
    });
    return out;
  }();
  return rows;
}
