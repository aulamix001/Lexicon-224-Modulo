#include "PresetBrowser.h"
namespace {
const juce::StringArray tagLabels{"Bright",  "Short", "Plate",      "Medium",
                                  "Dark",    "Hall",  "Drums",      "Long",
                                  "Best Of", "Room",  "Modulation", "Vocal"};
const juce::StringArray tagKeys{"Bright",   "Short", "Plate",     "Medium",
                                "Dark",     "Hall",  "Drums",     "Long",
                                "Best off", "Room",  "modulacao", "Vocal"};
} // namespace
PresetBrowser::Skin::Skin() {
  setColour(juce::TextButton::textColourOffId, juce::Colour(0xffdedede));
  setColour(juce::TextButton::textColourOnId, juce::Colours::white);
  setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff1b1a16));
  setColour(juce::TextEditor::textColourId, juce::Colour(0xffdddddd));
  setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
  setColour(juce::ScrollBar::thumbColourId, juce::Colour(0xff49473a));
}
void PresetBrowser::Skin::drawButtonBackground(juce::Graphics &g,
                                               juce::Button &b,
                                               const juce::Colour &, bool hover,
                                               bool down) {
  if ((b.getProperties().contains("symbol") ||
       b.getProperties().contains("tab")))
    return;
  const bool action = b.getProperties().contains("action");
  const bool selected = b.getToggleState();
  auto r = b.getLocalBounds().toFloat().reduced(1.5f);
  juce::Path outline;
  outline.addRoundedRectangle(r, 1.5f);
  // Frontal 2.5D relief: linear light, a 1 px lip and soft contact shadow.
  juce::DropShadow(juce::Colour(0x55000000), 2, {0, 1}).drawForPath(g, outline);
  auto top = juce::Colour(action     ? 0xff4c4c47
                          : selected ? 0xff515047
                                     : 0xff292820);
  auto bottom = juce::Colour(action     ? 0xff3d3d39
                             : selected ? 0xff414037
                                        : 0xff201f1a);
  if (down) {
    top = top.darker(.22f);
    bottom = bottom.darker(.12f);
  } else if (hover && b.isEnabled()) {
    top = top.brighter(.08f);
  }
  g.setGradientFill(
      juce::ColourGradient(top, 0, r.getY(), bottom, 0, r.getBottom(), false));
  g.fillPath(outline);
  g.setColour(juce::Colour(action || selected ? 0xff66655c : 0xff171611));
  g.drawRoundedRectangle(r, 1.5f, 1);
  g.setColour(juce::Colours::white.withAlpha(down                 ? .035f
                                             : action || selected ? .13f
                                                                  : .055f));
  g.drawLine(r.getX() + 1, r.getY() + 1, r.getRight() - 1, r.getY() + 1, 1);
  g.setColour(juce::Colours::black.withAlpha(down ? .35f : .22f));
  g.drawLine(r.getX() + 1, r.getBottom() - 1, r.getRight() - 1,
             r.getBottom() - 1, 1);
}
void PresetBrowser::Skin::drawButtonText(juce::Graphics &g, juce::TextButton &b,
                                         bool hover, bool down) {
  auto kind = b.getProperties()["symbol"].toString();
  if (kind.isEmpty()) {
    juce::ignoreUnused(hover, down);
    g.setFont(juce::Font(12, juce::Font::bold));
    const bool inactiveTab =
        b.getProperties().contains("tab") && !b.getToggleState();
    g.setColour(juce::Colour(inactiveTab ? 0xff9b9e9f : 0xffe1e1df)
                    .withAlpha(b.isEnabled() ? 1.f : .22f));
    g.drawText(b.getButtonText(), b.getLocalBounds().reduced(3),
               juce::Justification::centred);
    return;
  }
  g.setColour(juce::Colour(0xffaab6ba).withAlpha(b.isEnabled() ? 1.f : .3f));
  float x = b.getWidth() * .5f, y = b.getHeight() * .5f;
  if (kind == "close") {
    g.drawLine(x - 5, y - 5, x + 5, y + 5, 2);
    g.drawLine(x + 5, y - 5, x - 5, y + 5, 2);
    return;
  }
  juce::Path path;
  for (int i = 0; i < 10; ++i) {
    double angle = juce::MathConstants<double>::pi * (i / 5. - .5);
    float radius = i % 2 ? 3.8f : 8.5f;
    float px = x + radius * float(std::cos(angle)),
          py = y + radius * float(std::sin(angle));
    if (i == 0)
      path.startNewSubPath(px, py);
    else
      path.lineTo(px, py);
  }
  path.closeSubPath();
  if (b.getToggleState())
    g.fillPath(path);
  else
    g.strokePath(path, juce::PathStrokeType(1));
}
PresetBrowser::PresetBrowser(Lexicon224Processor &p) : processor(p) {
  setLookAndFeel(&skin);
  setSize(390, 960);
  search.setIndents(34, 9);
  search.setTooltip("Search presets and tags");
  close.getProperties().set("symbol", "close");
  favourites.getProperties().set("symbol", "star");
  star.getProperties().set("symbol", "star");
  close.setTooltip("Close preset browser");
  favourites.setTooltip("Show favourites");
  star.setTooltip("Favourite selected preset");
  search.onTextChange = [this] { refresh(); };
  addAndMakeVisible(search);
  for (auto *b : {&close, &favourites, &star, &save, &rename, &remove})
    addAndMakeVisible(*b);
  close.onClick = [this] {
    if (onClose)
      onClose();
  };
  favourites.onClick = [this] {
    onlyFav = !onlyFav;
    favourites.setToggleState(onlyFav, juce::dontSendNotification);
    refresh();
  };
  star.onClick = [this] {
    int i = current();
    if (i >= 0) {
      if (!library.toggleFavourite(library.entries()[size_t(i)].id))
        feedback.setText("Falha ao salvar favorito",
                         juce::dontSendNotification);
      refresh();
    }
  };
  for (auto *action : {&save, &rename, &remove})
    action->getProperties().set("action", true);
  save.onClick = [this] { editName(false); };
  rename.onClick = [this] { editName(true); };
  remove.onClick = [this] {
    int i = current();
    if (i >= 0) {
      feedback.setText(library.remove(i) ? "Preset de usuario excluido"
                                         : "Falha ao excluir preset",
                       juce::dontSendNotification);
      refresh();
    }
  };
  for (int i = 0; i < 12; ++i) {
    auto &b = tags[size_t(i)];
    b.setButtonText(tagLabels[i]);
    b.onClick = [this, i] {
      tag = tag == tagKeys[i] ? juce::String{} : tagKeys[i];
      for (int n = 0; n < 12; ++n)
        tags[size_t(n)].setToggleState(tag == tagKeys[n],
                                       juce::dontSendNotification);
      refresh();
    };
    addAndMakeVisible(b);
  }
  for (int i = 0; i < 3; ++i) {
    auto &b = tabs[size_t(i)];
    b.setButtonText(juce::StringArray{"TYPE", "ARTIST", "ALL"}[i]);
    b.getProperties().set("tab", true);
    b.onClick = [this, i] {
      tab = i;
      tag.clear();
      for (auto &t : tags)
        t.setToggleState(false, juce::dontSendNotification);
      refresh();
      resized();
      repaint();
    };
    addAndMakeVisible(b);
  }
  list.setRowHeight(33);
  list.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xff1b1a16));
  list.setOutlineThickness(0);
  addAndMakeVisible(list);
  title.setJustificationType(juce::Justification::centred);
  title.setFont(juce::Font(12, juce::Font::bold));
  title.setColour(juce::Label::textColourId, juce::Colour(0xffe5e5e5));
  addAndMakeVisible(title);
  feedback.setFont(juce::Font(10));
  feedback.setColour(juce::Label::textColourId, juce::Colour(0xffc4b98c));
  addAndMakeVisible(feedback);
  feedback.setText("55 presets - reconstrucoes visuais estimadas",
                   juce::dontSendNotification);
  refresh();
  lastRevision = processor.presetRevision.load();
  startTimerHz(10);
}
PresetBrowser::~PresetBrowser() {
  stopTimer();
  setLookAndFeel(nullptr);
}
void PresetBrowser::timerCallback() {
  auto revision = processor.presetRevision.load();
  if (revision != lastRevision) {
    lastRevision = revision;
    refresh();
  }
}
int PresetBrowser::current() const {
  int row = list.getSelectedRow();
  return juce::isPositiveAndBelow(row, visible.size()) ? visible[row] : -1;
}
void PresetBrowser::refresh() {
  for (int i = 0; i < 3; ++i)
    tabs[size_t(i)].setToggleState(i == tab, juce::dontSendNotification);
  auto selected = current();
  juce::String id =
      juce::isPositiveAndBelow(selected, int(library.entries().size()))
          ? library.entries()[size_t(selected)].id
          : juce::String{};
  auto activeId =
      processor.state.state.getProperty("factoryPresetId").toString();
  if (activeId.isNotEmpty()) {
    id = activeId;
    for (const auto &entry : library.entries())
      if (entry.id == activeId)
        title.setText(entry.name, juce::dontSendNotification);
  }
  list.deselectAllRows();
  visible = library.filter(search.getText(), tag, onlyFav);
  if (tab == 1)
    visible.clear();
  list.updateContent();
  int row = -1;
  for (int i = 0; i < visible.size(); ++i)
    if (library.entries()[size_t(visible[i])].id == id)
      row = i;
  if (row >= 0)
    list.selectRow(row);
  else {
    if (title.getText().isEmpty())
      title.setText("Default", juce::dontSendNotification);
    star.setEnabled(false);
    rename.setEnabled(false);
    remove.setEnabled(false);
  }
  if (tab == 1)
    feedback.setText("Sem metadados de artista nas referencias",
                     juce::dontSendNotification);
  list.repaint();
}
void PresetBrowser::selectedRowsChanged(int row) {
  if (!juce::isPositiveAndBelow(row, visible.size()))
    return;
  auto &e = library.entries()[size_t(visible[row])];
  title.setText(e.name, juce::dontSendNotification);
  star.setEnabled(true);
  star.setToggleState(library.favourite(e.id), juce::dontSendNotification);
  rename.setEnabled(e.user);
  remove.setEnabled(e.user);
  feedback.setText(
      e.user ? "Enter ou duplo clique: carregar estado completo"
             : "Enter ou duplo clique: carregar reconstrucao visual estimada",
      juce::dontSendNotification);
}
void PresetBrowser::returnKeyPressed(int row) {
  if (juce::isPositiveAndBelow(row, visible.size()))
    feedback.setText(library.apply(visible[row], processor),
                     juce::dontSendNotification);
}
void PresetBrowser::paintListBoxItem(int row, juce::Graphics &g, int w, int h,
                                     bool selected) {
  if (!juce::isPositiveAndBelow(row, visible.size()))
    return;
  auto &e = library.entries()[size_t(visible[row])];
  if (selected) {
    g.setColour(juce::Colour(0xff444238));
    g.fillRect(0, 0, w, h);
  }
  g.setColour(juce::Colour(0xffdddddd));
  g.setFont(juce::Font(11, juce::Font::bold));
  g.drawText(e.name, 10, 0, w - 35, h, juce::Justification::centredLeft);
  if (library.favourite(e.id)) {
    g.setColour(juce::Colour(0xffc9ba81));
    g.fillEllipse(float(w - 19), float(h / 2 - 2), 4, 4);
  }
}
void PresetBrowser::editName(bool isRename) {
  int index = current();
  if (isRename && (index < 0 || !library.entries()[size_t(index)].user))
    return;
  auto *dialog = new juce::AlertWindow(
      isRename ? "RENAME PRESET" : "SAVE PRESET", "Nome do preset de usuario",
      juce::MessageBoxIconType::NoIcon);
  dialog->addTextEditor("name", isRename ? library.entries()[size_t(index)].name
                                         : "User Preset");
  dialog->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
  dialog->addButton("CANCEL", 0, juce::KeyPress(juce::KeyPress::escapeKey));
  juce::Component::SafePointer<PresetBrowser> safe(this);
  dialog->enterModalState(
      true,
      juce::ModalCallbackFunction::create([safe, dialog, isRename,
                                           index](int result) {
        if (!safe || result != 1)
          return;
        auto name = dialog->getTextEditorContents("name");
        bool ok = isRename
                      ? safe->library.rename(index, name)
                      : safe->library.save(name, safe->processor, safe->tag);
        safe->refresh();
        safe->feedback.setText(ok ? "Preset salvo" : "Falha ao salvar preset",
                               juce::dontSendNotification);
      }),
      true);
}
void PresetBrowser::paint(juce::Graphics &g) {
  g.fillAll(juce::Colour(0xff24231e));
  auto frame = getLocalBounds().toFloat().reduced(1);
  g.setGradientFill(juce::ColourGradient(juce::Colour(0xff49473d), 0, 0,
                                         juce::Colour(0xff3e3c33), 0,
                                         float(getHeight()), false));
  g.fillRoundedRectangle(frame, 3);
  g.setColour(juce::Colour(0xff666357));
  g.drawRoundedRectangle(frame.reduced(.5f), 3, 1);
  g.setColour(juce::Colour(0xff1a1915));
  g.drawRoundedRectangle(frame, 3, 1);
  g.setColour(juce::Colour(0xff302e27));
  g.fillRoundedRectangle(4, 78, float(getWidth() - 8), float(getHeight() - 82),
                         2);
  g.setColour(juce::Colour(0xff171611));
  g.drawRoundedRectangle(4, 38, float(getWidth() - 10), 38, 2, 1);
  g.setColour(juce::Colour(0x18ffffff));
  g.drawLine(6, 76, float(getWidth() - 8), 76, 1);
  g.setColour(juce::Colour(0xffdddddd));
  g.setFont(juce::Font(12, juce::Font::bold));
  g.drawText("LEXICON 224 PRESETS", 20, 4, getWidth() - 40, 29,
             juce::Justification::centred);
  g.setFont(juce::Font(11, juce::Font::bold));
  juce::Path triangle;
  triangle.addTriangle(51, 105, 57, 99, 57, 105);
  g.setColour(juce::Colour(0xff77796e));
  g.fillPath(triangle);
  g.setColour(juce::Colour(0xffdddddd));
  g.drawText("TAGS", 14, 88, 70, 21, juce::Justification::centredLeft);
  g.drawText("FAVORITES", getWidth() - 128, 88, 87, 21,
             juce::Justification::centredRight);
  g.setColour(juce::Colour(0xff5a584d));
  g.fillRect(9, 117, getWidth() - 18, 2);
  g.setColour(juce::Colour(0xffdededc));
  g.fillRect(9 + tab * (getWidth() - 18) / 3, 117, (getWidth() - 18) / 3, 2);
  g.setColour(juce::Colour(0xff151510));
  g.drawRoundedRectangle(5, 292, float(getWidth() - 10),
                         float(getHeight() - 318), 2, 2);
}
void PresetBrowser::resized() {
  int w = getWidth();
  close.setBounds(w - 31, 4, 26, 27);
  search.setBounds(5, 38, w - 12, 37);
  favourites.setBounds(w - 38, 86, 26, 25);
  for (int i = 0; i < 3; ++i)
    tabs[size_t(i)].setBounds(8 + i * (w - 16) / 3, 122, (w - 16) / 3, 26);
  for (int i = 0; i < 12; ++i) {
    tags[size_t(i)].setVisible(tab != 1);
    tags[size_t(i)].setBounds(8 + (i % 3) * (w - 29) / 3, 151 + (i / 3) * 34,
                              (w - 35) / 3, 30);
  }
  title.setBounds(12, 298, w - 55, 27);
  star.setBounds(w - 48, 298, 30, 27);
  int bw = (w - 38) / 3;
  remove.setBounds(14, 329, bw, 30);
  rename.setBounds(19 + bw, 329, bw, 30);
  save.setBounds(24 + bw * 2, 329, bw, 30);
  list.setBounds(8, 370, w - 16, getHeight() - 400);
  feedback.setBounds(8, getHeight() - 27, w - 16, 23);
}
void PresetBrowser::mouseDown(const juce::MouseEvent &e) {
  if (e.y < 34)
    dragger.startDraggingComponent(getTopLevelComponent(), e);
}
void PresetBrowser::mouseDrag(const juce::MouseEvent &e) {
  if (e.getMouseDownY() < 34)
    dragger.dragComponent(getTopLevelComponent(), e, nullptr);
}
PresetWindow::PresetWindow(Lexicon224Processor &p)
    : DocumentWindow("LEXICON 224 PRESETS", juce::Colour(0xff302e27), 0,
                     false) {
  setUsingNativeTitleBar(false);
  setTitleBarHeight(0);
  setResizable(false, false);
  auto *browser = new PresetBrowser(p);
  browser->onClose = [this] { setVisible(false); };
  setContentOwned(browser, true);
  addToDesktop(juce::ComponentPeer::windowIsTemporary |
               juce::ComponentPeer::windowHasDropShadow);
}
void PresetWindow::showBeside(juce::Component &editor) {
  auto anchor = editor.getScreenBounds();
  auto *display =
      juce::Desktop::getInstance().getDisplays().getDisplayForRect(anchor);
  auto area =
      display ? display->userArea : juce::Rectangle<int>(0, 0, 1920, 1080);
  setSize(390, juce::jmin(960, area.getHeight()));
  int x = anchor.getRight() + 8;
  if (x + getWidth() > area.getRight())
    x = anchor.getX() - getWidth() - 8;
  setTopLeftPosition(
      juce::jlimit(area.getX(),
                   juce::jmax(area.getX(), area.getRight() - getWidth()), x),
      juce::jlimit(area.getY(),
                   juce::jmax(area.getY(), area.getBottom() - getHeight()),
                   anchor.getY()));
  setVisible(true);
  toFront(true);
}
