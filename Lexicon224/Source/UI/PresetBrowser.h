#pragma once
#include "../Programs/PresetLibrary.h"
class PresetBrowser : public juce::Component, private juce::ListBoxModel {
public:
  explicit PresetBrowser(Lexicon224Processor &);
  void paint(juce::Graphics &) override;
  void resized() override;
  void mouseDown(const juce::MouseEvent &) override;
  void mouseDrag(const juce::MouseEvent &) override;
  std::function<void()> onClose;

private:
  class Skin : public juce::LookAndFeel_V4 {
  public:
    Skin();
    int getDefaultScrollbarWidth() override { return 9; }
    juce::Font getTextButtonFont(juce::TextButton &, int) override {
      return juce::Font(11, juce::Font::bold);
    }
    void drawButtonText(juce::Graphics &, juce::TextButton &, bool,
                        bool) override;
    void drawButtonBackground(juce::Graphics &, juce::Button &,
                              const juce::Colour &, bool, bool) override;
  };
  int getNumRows() override { return visible.size(); }
  void paintListBoxItem(int, juce::Graphics &, int, int, bool) override;
  void listBoxItemDoubleClicked(int row, const juce::MouseEvent &) override {
    returnKeyPressed(row);
  }
  void selectedRowsChanged(int) override;
  void returnKeyPressed(int) override;
  void refresh();
  int current() const;
  void editName(bool);
  Lexicon224Processor &processor;
  PresetLibrary library;
  Skin skin;
  class SearchEditor : public juce::TextEditor {
    void paintOverChildren(juce::Graphics &g) override {
      g.setColour(juce::Colour(0xff929b9d));
      g.drawEllipse(11, 11, 11, 11, 1.5f);
      g.drawLine(20, 21, 26, 27, 1.5f);
    }
  } search;
  juce::TextButton close{""}, favourites{""}, star{""}, save{"SAVE"},
      rename{"RENAME"}, remove{"DELETE"};
  std::array<juce::TextButton, 12> tags;
  std::array<juce::TextButton, 3> tabs;
  juce::ListBox list{"Presets", this};
  juce::Label title, feedback;
  juce::Array<int> visible;
  juce::String tag;
  bool onlyFav = false;
  int tab = 0;
  juce::ComponentDragger dragger;

public:
  ~PresetBrowser() override;
};
class PresetWindow : public juce::DocumentWindow {
public:
  explicit PresetWindow(Lexicon224Processor &);
  void closeButtonPressed() override { setVisible(false); }
  void showBeside(juce::Component &);
};
