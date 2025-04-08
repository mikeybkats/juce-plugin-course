#include "Tremolo/PluginEditor.h"
#include <juce_graphics/juce_graphics.h>
#include "Tremolo/PluginProcessor.h"

namespace ws {
namespace Colours {
enum : size_t { ORANGE, LIGHT_GREY };
}

namespace {
const auto& getColourPalette() {
  static const std::array COLOURS{
      juce::Colour{0xFFEF7600},
      juce::Colour{0xFFD9D9D9},
  };
  return COLOURS;
}
}  // namespace

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p),
      waveformAttachment{p.getParameters().waveform, waveformComboBox},
      rateAttachment{p.getParameters().rate, rateSlider},
      bypassAttachment{p.getParameters().bypassed, bypassButton},
      lfoVisualizer{
          [&p](juce::AudioBuffer<float>& b) { p.readAllLfoSamples(b); },
          [&p] { return p.getSampleRateThreadSafe(); }} {
  lookAndFeel.setColour(juce::TextButton::buttonOnColourId,
                        std::get<Colours::ORANGE>(getColourPalette()));
  lookAndFeel.setColour(juce::TextButton::buttonColourId,
                        std::get<Colours::LIGHT_GREY>(getColourPalette()));

  setLookAndFeel(&lookAndFeel);

  background.topColour = juce::Colours::whitesmoke;
  background.bottomColour = juce::Colours::grey;
  addAndMakeVisible(background);

  headerLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(headerLabel);

  waveformLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(waveformLabel);

  waveformComboBox.addItemList(p.getParameters().waveform.choices, 1);
  waveformAttachment.sendInitialUpdate();
  addAndMakeVisible(waveformComboBox);

  rateLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(rateLabel);
  rateSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
  rateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox,
                             true, 0, 0);
  addAndMakeVisible(rateSlider);

  bypassLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(bypassLabel);

  addAndMakeVisible(bypassButton);

  addAndMakeVisible(lfoVisualizer);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(600, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void AudioPluginAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();

  background.setBounds(bounds);

  bounds.removeFromTop(28);
  headerLabel.setBounds(bounds.removeFromTop(18));

  bounds.removeFromTop(12);

  bounds.removeFromLeft(26);
  bounds.removeFromRight(26);

  auto labelsBounds = bounds.removeFromTop(18);
  const auto oneThirdOfWidth = labelsBounds.getWidth() / 3;
  waveformLabel.setBounds(labelsBounds.removeFromLeft(oneThirdOfWidth));
  bypassLabel.setBounds(labelsBounds.removeFromRight(oneThirdOfWidth));
  rateLabel.setBounds(labelsBounds);

  auto widgetBounds = bounds.removeFromTop(67);

  waveformComboBox.setBounds(widgetBounds.removeFromLeft(oneThirdOfWidth));

  auto bypassButtonBounds = widgetBounds.removeFromRight(oneThirdOfWidth);
  bypassButtonBounds.reduce(8, 14);
  bypassButton.setBounds(bypassButtonBounds);

  rateSlider.setBounds(widgetBounds.reduced(6));

  bounds.removeFromTop(12);
  bounds.removeFromBottom(16);

  lfoVisualizer.setBounds(bounds);
}
}  // namespace ws
