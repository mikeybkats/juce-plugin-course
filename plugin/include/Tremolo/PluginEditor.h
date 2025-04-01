#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Gradient.h"

namespace ws {

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
  ~AudioPluginAudioProcessorEditor() override;

  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  AudioPluginAudioProcessor& processorRef;
  Gradient background;
  juce::Label headerLabel{"header label", "TREMOLO"};

  juce::Label waveformLabel{"waveform label", "WAVEFORM"};
  juce::ComboBox waveformComboBox;

  juce::Label rateLabel{"rate label", "RATE"};
  juce::Slider rateSlider;

  juce::Label bypassLabel{"bypass label", "BYPASS"};
  juce::TextButton bypassButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
}  // namespace ws
