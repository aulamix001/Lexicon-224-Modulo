#include "PresetLibrary.h"
#include <PresetData.h>
PresetLibrary::PresetLibrary(juce::File storage) : file(storage) {
  if (file == juce::File{})
    file =
        juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
            .getChildFile("AulaMix/Lexicon224/presets.json");
  auto catalog = juce::JSON::parse(juce::String::fromUTF8(
      PresetData::catalog_json, PresetData::catalog_jsonSize));
  if (auto *rows = catalog["presets"].getArray())
    for (auto row : *rows) {
      if (!row["name"].isString())
        continue;
      Entry e;
      e.id = row["id"].toString();
      e.name = row["name"].toString();
      e.data = row;
      if (auto *tags = row["tags"].getArray())
        for (auto t : *tags)
          e.tags.add(t.toString());
      items.push_back(std::move(e));
    }
  auto saved = juce::JSON::parse(file.loadFileAsString());
  if (auto *fav = saved["favourites"].getArray())
    for (auto id : *fav)
      favourites.addIfNotAlreadyThere(id.toString());
  if (auto *users = saved["users"].getArray())
    for (auto row : *users) {
      if (!row["name"].isString() || !row["xml"].isString())
        continue;
      Entry e;
      e.id = row["id"].toString();
      e.name = row["name"].toString();
      e.data = row;
      e.user = true;
      if (auto *tags = row["tags"].getArray())
        for (auto t : *tags)
          e.tags.add(t.toString());
      items.push_back(std::move(e));
    }
}
juce::Array<int> PresetLibrary::filter(const juce::String &query,
                                       const juce::String &tag,
                                       bool onlyFav) const {
  juce::Array<int> out;
  for (size_t i = 0; i < items.size(); ++i) {
    auto &e = items[i];
    if ((tag.isEmpty() || e.tags.contains(tag)) &&
        (!onlyFav || favourite(e.id)) &&
        (query.isEmpty() || e.name.containsIgnoreCase(query) ||
         e.tags.joinIntoString(" ").containsIgnoreCase(query)))
      out.add(int(i));
  }
  std::sort(out.begin(), out.end(), [this](int a, int b) {
    return items[size_t(a)].name.compareNatural(items[size_t(b)].name) < 0;
  });
  return out;
}
bool PresetLibrary::favourite(const juce::String &id) const {
  return favourites.contains(id);
}
bool PresetLibrary::toggleFavourite(const juce::String &id) {
  auto old = favourites;
  if (favourite(id))
    favourites.removeString(id);
  else
    favourites.add(id);
  if (persist())
    return true;
  favourites = old;
  return false;
}
juce::String PresetLibrary::apply(int index, Lexicon224Processor &processor) {
  if (!juce::isPositiveAndBelow(index, int(items.size())))
    return "Selecione um preset";
  auto &e = items[size_t(index)];
  if (e.user) {
    auto xml = juce::parseXML(e.data["xml"].toString());
    if (!xml || !xml->hasTagName("Lexicon224"))
      return "Estado de usuario invalido";
    processor.state.replaceState(juce::ValueTree::fromXml(*xml));
    ++processor.presetRevision;
    return "Preset de usuario carregado · estado completo";
  }
  const auto mapping = e.data["confirmed_plugin_parameters"];
  auto *object = mapping.getDynamicObject();
  if (!object || !object->hasProperty("program"))
    return "Algoritmo ausente ou nao identificado · audio mantido";
  const bool conflict = e.data["modifier_mapping_status"].toString() ==
                        "conflicting_source_observations";
  // Intentionally do not call setCurrentProgram: unknown faders must not be
  // replaced by factory defaults.
  for (const auto &property : object->getProperties()) {
    if (conflict && (property.name == juce::Identifier("mode") ||
                     property.name == juce::Identifier("optimize")))
      continue;
    if (auto *parameter =
            processor.state.getParameter(property.name.toString())) {
      parameter->beginChangeGesture();
      parameter->setValueNotifyingHost(
          parameter->convertTo0to1(float(property.value)));
      parameter->endChangeGesture();
    }
  }
  ++processor.presetRevision;
  return conflict ? "Programa aplicado · LEDs divergentes; faders mantidos"
                  : "Mapeamento parcial · faders e valores ocultos mantidos";
}
bool PresetLibrary::persist() {
  auto object = std::make_unique<juce::DynamicObject>();
  juce::Array<juce::var> fav, users;
  for (auto id : favourites)
    fav.add(id);
  for (auto &e : items)
    if (e.user)
      users.add(e.data);
  object->setProperty("favourites", fav);
  object->setProperty("users", users);
  if (file.getParentDirectory().createDirectory().failed())
    return false;
  return file.replaceWithText(
      juce::JSON::toString(juce::var(object.release())));
}
bool PresetLibrary::save(const juce::String &name, Lexicon224Processor &p,
                         const juce::String &tag) {
  if (name.trim().isEmpty())
    return false;
  Entry e;
  e.id = "user-" + juce::Uuid().toString();
  e.name = name.trim();
  e.user = true;
  if (tag.isNotEmpty())
    e.tags.add(tag);
  auto data = std::make_unique<juce::DynamicObject>();
  data->setProperty("id", e.id);
  data->setProperty("name", e.name);
  juce::Array<juce::var> tags;
  for (auto t : e.tags)
    tags.add(t);
  data->setProperty("tags", tags);
  data->setProperty("xml", p.state.copyState().createXml()->toString());
  e.data = juce::var(data.release());
  items.push_back(e);
  if (persist())
    return true;
  items.pop_back();
  return false;
}
bool PresetLibrary::rename(int i, const juce::String &name) {
  if (!juce::isPositiveAndBelow(i, int(items.size())) ||
      !items[size_t(i)].user || name.trim().isEmpty())
    return false;
  auto &e = items[size_t(i)];
  auto old = e.name;
  e.name = name.trim();
  e.data.getDynamicObject()->setProperty("name", e.name);
  if (persist())
    return true;
  e.name = old;
  e.data.getDynamicObject()->setProperty("name", old);
  return false;
}
bool PresetLibrary::remove(int i) {
  if (!juce::isPositiveAndBelow(i, int(items.size())) || !items[size_t(i)].user)
    return false;
  auto old = items[size_t(i)];
  items.erase(items.begin() + i);
  if (persist())
    return true;
  items.insert(items.begin() + i, old);
  return false;
}
