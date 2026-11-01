#include "PluginProcessor.h"
#include "Programs/PresetLibrary.h"
#include <iostream>
#include <stdexcept>
void check(bool b, const char *m) {
  if (!b)
    throw std::runtime_error(m);
}
void set(Lexicon224Processor &p, const char *id, float v) {
  auto *a = p.state.getParameter(id);
  a->setValueNotifyingHost(a->convertTo0to1(v));
}
int main() {
  try {
    Lexicon224Processor p;
    p.prepareToPlay(48000, 512);
    juce::MidiBuffer midi;
    set(p, "mix", 0);
    p.prepareToPlay(48000, 512);
    for (int size : {1, 17, 127, 512, 1024}) {
      juce::AudioBuffer<float> b(2, size);
      for (int n = 0; n < size; ++n) {
        b.setSample(0, n, .2f);
        b.setSample(1, n, -.1f);
      }
      p.processBlock(b, midi);
      for (int n = 0; n < size; ++n)
        check(std::abs(b.getSample(0, n) - .2f) < 1e-6f &&
                  std::abs(b.getSample(1, n) + .1f) < 1e-6f,
              "Dry path altered");
    }
    for (int i = 0; i < 55; ++i) {
      p.setCurrentProgram(i);
      check(p.getCurrentProgram() == i, "Factory recall failed");
    }
    set(p, "bass", 7.5f);
    p.saveRegister(0);
    p.saveRegister(35);
    set(p, "bass", 1);
    check(p.recallRegister(0), "Main register lost");
    check(std::abs(p.state.getRawParameterValue("bass")->load() - 7.5f) < .001,
          "Register value wrong");
    check(!p.recallRegister(12), "Empty register unexpectedly populated");
    for (int slot = 0; slot < 36; ++slot) {
      set(p, "bass", 1.f + float(slot));
      set(p, "modeRate", 1.f + float(slot % 16));
      set(p, "converter", float(slot % 3));
      p.saveRegister(slot);
    }
    juce::MemoryBlock state;
    p.getStateInformation(state);
    Lexicon224Processor restored;
    restored.setStateInformation(state.getData(), int(state.getSize()));
    check(restored.recallRegister(35), "Extended register not serialized");
    for (int slot = 0; slot < 36; ++slot) {
      check(restored.recallRegister(slot),
            "Register missing after serialization");
      check(std::abs(restored.state.getRawParameterValue("bass")->load() -
                     (1.f + float(slot))) < .001f,
            "Restored register value mismatch");
      check(int(restored.state.getRawParameterValue("converter")->load()) ==
                slot % 3,
            "Converter model not included in register");
    }
    set(p, "mix", 100);
    p.prepareToPlay(48000, 512);
    double energy = 0;
    for (int block = 0; block < 120; ++block) {
      if (block == 45)
        p.setCurrentProgram(1);
      juce::AudioBuffer<float> b(2, 512);
      b.clear();
      if (block == 0)
        b.setSample(0, 0, .5f);
      p.processBlock(b, midi);
      for (int ch = 0; ch < 2; ++ch)
        for (int n = 0; n < 512; ++n) {
          float v = b.getSample(ch, n);
          check(std::isfinite(v), "Plugin nonfinite output");
          energy += v * v;
        }
    }
    check(energy > 1e-6, "Plugin wet path silent");
    // Converter changes must remain finite and work at every supported rate.
    for (double rate : {44100., 48000., 88200., 96000.}) {
      p.prepareToPlay(rate, 257);
      for (int profile = 0; profile < 3; ++profile) {
        set(p, "converter", float(profile));
        for (int block = 0; block < 20; ++block) {
          juce::AudioBuffer<float> b(2, 257);
          b.clear();
          b.setSample(0, 0, .25f);
          p.processBlock(b, midi);
          for (int ch = 0; ch < 2; ++ch)
            for (int n = 0; n < 257; ++n)
              check(std::isfinite(b.getSample(ch, n)),
                    "Converter switching produced nonfinite audio");
        }
      }
    }
    auto buses = p.getBusesLayout();
    buses.outputBuses.set(1, juce::AudioChannelSet::stereo());
    check(p.setBusesLayout(buses), "Quad bus layout rejected");
    p.prepareToPlay(48000, 512);
    juce::AudioBuffer<float> quad(4, 512);
    quad.clear();
    quad.setSample(0, 0, .5f);
    p.processBlock(quad, midi);
    double auxiliaryEnergy = 0;
    for (int block = 0; block < 60; ++block) {
      quad.clear();
      if (block == 0)
        quad.setSample(0, 0, .5f);
      p.processBlock(quad, midi);
      for (int ch = 2; ch < 4; ++ch)
        for (int n = 0; n < 512; ++n)
          auxiliaryEnergy += quad.getSample(ch, n) * quad.getSample(ch, n);
    }
    check(auxiliaryEnergy > 1e-7, "Quad return bus silent");
    buses.inputBuses.set(0, juce::AudioChannelSet::mono());
    check(p.setBusesLayout(buses), "Mono input rejected");
    p.prepareToPlay(48000, 512);
    quad.clear();
    quad.setSample(0, 0, .5f);
    p.processBlock(quad, midi);
    const auto directory =
        juce::File::getSpecialLocation(juce::File::tempDirectory)
            .getChildFile("lex224-test-" + juce::Uuid().toString());
    const auto storage = directory.getChildFile("presets.json");
    {
      PresetLibrary library(storage);
      check(library.entries().size() == 55,
            "Named screenshot catalog count incorrect");
      check(library.filter("", "Bright", false).size() == 26,
            "Bright filter incorrect");
      check(library.filter("", "Medium", false).size() == 19,
            "Medium filter incorrect");
      check(library.filter("brooklyn", "", false).size() == 1, "Search failed");
      int vocal = library.filter("Ballad Vocals", "", false)[0];
      auto id = library.entries()[size_t(vocal)].id;
      check(library.toggleFavourite(id), "Favorite persistence failed");
      check(library.filter("", "", true).size() == 1,
            "Favorites filter failed");
      set(p, "bass", 12);
      set(p, "mid", 7);
      library.apply(vocal, p);
      const auto loadedBass = p.state.getRawParameterValue("bass")->load();
      const auto loadedMid = p.state.getRawParameterValue("mid")->load();
      check(std::abs(loadedBass - 12) > .01 && std::abs(loadedMid - 7) > .01,
            "Visual reconstruction failed to set faders");
      set(p, "bass", 60);
      set(p, "modeRate", 16);
      library.apply(vocal, p);
      check(std::abs(p.state.getRawParameterValue("bass")->load() -
                     loadedBass) < .001 &&
                p.state.getRawParameterValue("modeRate")->load() < 3,
            "Photo state depends on prior controls");
      check(p.getCurrentAlgorithm() == 1, "Confirmed algorithm not mapped");
      int density = library.filter("Density Plate", "", false)[0];
      library.apply(density, p);
      check(p.getCurrentAlgorithm() == 7, "Constant Plate is not selectable");
      int room = library.filter("Live Room", "", false)[0];
      library.apply(room, p);
      check(p.getCurrentAlgorithm() == 6, "Room A is not selectable");
      int chorus = library.filter("Vocal Wide Chorus", "", false)[0];
      library.apply(chorus, p);
      check(p.getCurrentAlgorithm() == 8, "Chorus is not selectable");
      int conflict = library.filter("Wider-1", "", false)[0];
      set(p, "optimize", 1);
      library.apply(conflict, p);
      check(p.state.getRawParameterValue("optimize")->load() < .5f,
            "Visual reconstruction did not use documented first reference");
      int reconstructed = 0, blocked = 0;
      for (size_t i = 0; i < library.entries().size(); ++i) {
        const auto &entry = library.entries()[i];
        const auto model = entry.data["visual_reconstruction"];
        const auto before = p.state.copyState().toXmlString();
        library.apply(int(i), p);
        if (bool(model["loadable_with_current_dsp"])) {
          ++reconstructed;
          for (const auto &property :
               model["parameters"].getDynamicObject()->getProperties()) {
            auto *value =
                p.state.getRawParameterValue(property.name.toString());
            check(value &&
                      std::abs(value->load() - float(property.value)) < .01f,
                  "Reconstructed state does not match its mapping");
          }
        } else {
          ++blocked;
          check(p.state.copyState().toXmlString() == before,
                "Unavailable algorithm modified the state");
        }
      }
      // Exercise the host API for every named preset, audio output and saved
      // identity.
      for (int index = 0; index < 55; ++index) {
        check(p.getProgramName(index).isNotEmpty(), "Host preset name missing");
        p.setCurrentProgram(index);
        check(p.getCurrentProgram() == index, "Host preset identity mismatch");
        const auto before = p.state.copyState();
        set(p, "bass", 69);
        p.setCurrentProgram(index);
        check(p.state.copyState().isEquivalentTo(before),
              "Host recall is not deterministic");
        p.prepareToPlay(48000, 257);
        double presetEnergy = 0;
        juce::AudioBuffer<float> audio(4, 257);
        audio.clear();
        audio.setSample(0, 0, .3f);
        for (int block = 0; block < 90; ++block) {
          if (block > 0)
            audio.clear();
          p.processBlock(audio, midi);
          for (int ch = 0; ch < 4; ++ch)
            for (int n = 0; n < 257; ++n) {
              check(std::isfinite(audio.getSample(ch, n)),
                    "Preset produced invalid audio");
              presetEnergy += audio.getSample(ch, n) * audio.getSample(ch, n);
            }
        }
        check(presetEnergy > 1e-12, "A named preset produced no wet audio");
        juce::MemoryBlock snapshot;
        p.getStateInformation(snapshot);
        restored.setStateInformation(snapshot.getData(),
                                     int(snapshot.getSize()));
        check(restored.getCurrentProgram() == index,
              "Preset identity did not persist");
      }
      check(reconstructed == 55 && blocked == 0,
            "Unexpected reconstruction counts");
      check(!library.remove(vocal) && !library.rename(vocal, "overwrite"),
            "Factory references must be protected");
      set(p, "bass", 12);
      check(library.save("Browser Test", p, "Bright"),
            "User preset save failed");
      PresetLibrary restoredLibrary(storage);
      check(restoredLibrary.favourite(id), "Favorite not restored");
      auto users = restoredLibrary.filter("Browser Test", "Bright", false);
      check(users.size() == 1, "User preset not restored");
      set(p, "bass", 1);
      restoredLibrary.apply(users[0], p);
      check(std::abs(p.state.getRawParameterValue("bass")->load() - 12) < .001,
            "Full user state recall failed");
      check(restoredLibrary.rename(users[0], "Renamed"), "Rename failed");
      check(restoredLibrary.remove(users[0]), "Delete failed");
      PresetLibrary finalLibrary(storage);
      check(finalLibrary.filter("Renamed", "", false).isEmpty(),
            "Deleted user preset returned");
    }
    directory.deleteRecursively();
    std::cout
        << "Dry path, adaptive blocks, factory presets, 36-register state, wet "
           "audio, preset transition, mono and quad layouts passed\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
