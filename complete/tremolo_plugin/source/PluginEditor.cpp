namespace ws {
PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p),
      waveformAttachment{p.getParameters().waveform, waveformComboBox},
      rateAttachment{p.getParameters().rate, rateSlider},
      bypassAttachment{p.getParameters().bypassed, bypassButton},
      lfoVisualizer{
          [&p](juce::AudioBuffer<float>& b) { p.readAllLfoSamples(b); },
          [&p] { return p.getSampleRateThreadSafe(); },
          [&p] { return p.getParameters().bypassed.get(); }} {
  background.setImage(juce::ImageCache::getFromMemory(
      assets::RenderedBackground_png, assets::RenderedBackground_pngSize));
  addAndMakeVisible(background);

  waveformLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(waveformLabel);

  auto waveformChoices = p.getParameters().waveform.choices;
  for (auto& choice : waveformChoices) {
    choice = choice.toUpperCase();
  }
  waveformComboBox.addItemList(waveformChoices, 1);
  waveformAttachment.sendInitialUpdate();
  addAndMakeVisible(waveformComboBox);

  rateSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
  rateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox,
                             true, 0, 0);
  rateSlider.setTextValueSuffix(" Hz");
  addAndMakeVisible(rateSlider);

  rateLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(rateLabel);

  bypassLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(bypassLabel);

  addAndMakeVisible(bypassButton);

  lfoVisualizer.setCurveWidth(2.f);
  lfoVisualizer.setCurveColor(
      lookAndFeel.getColor(CustomLookAndFeel::Colors::orange));
  lfoVisualizer.setBackgroundColor(juce::Colours::transparentBlack);
  addAndMakeVisible(lfoVisualizer);

  // set the look and feel AFTER configuring all child components
  setLookAndFeel(&lookAndFeel);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(540, 270);
}

PluginEditor::~PluginEditor() {
  setLookAndFeel(nullptr);
}

void PluginEditor::resized() {
  auto bounds = getLocalBounds();

  background.setBounds(bounds);

  auto lfoVisualizerBounds = bounds.reduced(18, 27);
  lfoVisualizerBounds.removeFromTop(122);
  lfoVisualizer.setBounds(lfoVisualizerBounds);

  auto rateSliderBounds = bounds.reduced(230, 40);
  rateSliderBounds.removeFromBottom(110);
  rateSlider.setBounds(rateSliderBounds);
  rateLabel.setBounds(rateSliderBounds);

  auto waveformComboBoxBounds = bounds;
  waveformComboBoxBounds.removeFromTop(66);
  waveformComboBoxBounds.removeFromRight(392);
  waveformComboBoxBounds.removeFromBottom(176);
  waveformComboBoxBounds.removeFromLeft(16);
  waveformComboBox.setBounds(waveformComboBoxBounds);

  auto bypassButtonBounds = bounds;
  bypassButtonBounds.removeFromTop(66);
  bypassButtonBounds.removeFromRight(16);
  bypassButtonBounds.removeFromBottom(176);
  bypassButtonBounds.removeFromLeft(392);
  bypassButton.setBounds(bypassButtonBounds);

  bounds.removeFromTop(58);

  constexpr auto widgetMargin = 26;
  bounds.removeFromLeft(widgetMargin);
  bounds.removeFromRight(widgetMargin);

  auto labelsBounds = bounds.removeFromTop(18);
  const auto oneThirdOfWidth = labelsBounds.getWidth() / 3;
  waveformLabel.setBounds(labelsBounds.removeFromLeft(oneThirdOfWidth));
  bypassLabel.setBounds(labelsBounds.removeFromRight(oneThirdOfWidth));
}
}  // namespace ws
