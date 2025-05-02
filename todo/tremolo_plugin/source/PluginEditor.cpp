namespace ws {
PluginEditor::PluginEditor(PluginProcessor& p) : AudioProcessorEditor(&p) {
  // set the look and feel AFTER configuring all child components
  setLookAndFeel(&lookAndFeel);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(600, 300);
}

PluginEditor::~PluginEditor() {
  // It's important to reset the LookAndFeel instance here because
  // it is destroyed before PluginEditor and its superclass are.
  setLookAndFeel(nullptr);
}

void PluginEditor::resized() {}
}  // namespace ws
