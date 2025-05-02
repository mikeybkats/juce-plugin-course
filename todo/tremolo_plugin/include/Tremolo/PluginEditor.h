#pragma once

namespace ws {
class PluginEditor : public juce::AudioProcessorEditor {
public:
  explicit PluginEditor(PluginProcessor&);
  ~PluginEditor() override;

  void resized() override;

private:
  CustomLookAndFeel lookAndFeel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
}  // namespace ws
