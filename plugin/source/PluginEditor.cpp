#include "Tremolo/PluginEditor.h"
#include <juce_graphics/juce_graphics.h>
#include "Tremolo/PluginProcessor.h"

namespace ws {
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p) {
  juce::ignoreUnused(processorRef);

  background.topColour = juce::Colours::whitesmoke;
  background.bottomColour = juce::Colours::grey;
  addAndMakeVisible(background);

  headerLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(headerLabel);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(600, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::resized() {
  background.setBounds(getLocalBounds());

  auto bounds = getLocalBounds();

  bounds.removeFromTop(28);
  headerLabel.setBounds(bounds.removeFromTop(18));
}
}  // namespace ws
