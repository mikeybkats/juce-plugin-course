#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Gradient.h"
#include "BypassButton.h"
#include "Tremolo/LfoVisualizer.h"

namespace ws {

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
  ~AudioPluginAudioProcessorEditor() override;

  void resized() override;

private:
  Gradient background;
  juce::Label headerLabel{"header label", "TREMOLO"};

  juce::Label waveformLabel{"waveform label", "WAVEFORM"};
  juce::ComboBox waveformComboBox;
  juce::ComboBoxParameterAttachment waveformAttachment;

  juce::Label rateLabel{"rate label", "RATE"};
  juce::Slider rateSlider;
  juce::SliderParameterAttachment rateAttachment;

  juce::Label bypassLabel{"bypass label", "BYPASS"};
  BypassButton bypassButton;
  juce::ButtonParameterAttachment bypassAttachment;

  LfoVisualizer lfoVisualizer;

  juce::LookAndFeel_V4 lookAndFeel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
}  // namespace ws
