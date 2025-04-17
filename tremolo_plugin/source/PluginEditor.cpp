namespace ws {
PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p),
      waveformAttachment{p.getParameters().waveform, waveformComboBox},
      rateAttachment{p.getParameters().rate, rateSlider},
      bypassAttachment{p.getParameters().bypassed, bypassButton},
      lfoVisualizer{
          [&p](juce::AudioBuffer<float>& b) { p.readAllLfoSamples(b); },
          [&p] { return p.getSampleRateThreadSafe(); }} {
  background.topColour =
      lookAndFeel.getColor(CustomLookAndFeel::Colors::lightGrey).brighter();
  background.bottomColour =
      lookAndFeel.getColor(CustomLookAndFeel::Colors::lightGrey).darker();
  addAndMakeVisible(background);

  headerLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(headerLabel);

  waveformLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(waveformLabel);

  auto waveformChoices = p.getParameters().waveform.choices;
  for (auto& choice : waveformChoices) {
    choice = choice.toUpperCase();
  }
  waveformComboBox.addItemList(waveformChoices, 1);
  waveformAttachment.sendInitialUpdate();
  addAndMakeVisible(waveformComboBox);

  rateLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(rateLabel);
  rateSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
  rateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow,
                             true, 60, 20);
  rateSlider.setTextValueSuffix(" Hz");
  addAndMakeVisible(rateSlider);

  bypassLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(bypassLabel);

  addAndMakeVisible(bypassButton);

  lfoVisualizer.setCurveColor(
      lookAndFeel.getColor(CustomLookAndFeel::Colors::orange));
  lfoVisualizer.setBackgroundColor(
      lookAndFeel.getColor(CustomLookAndFeel::Colors::lightGrey));
  addAndMakeVisible(lfoVisualizer);

  // set the look and feel AFTER configuring all child components
  setLookAndFeel(&lookAndFeel);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(600, 300);
}

PluginEditor::~PluginEditor() {
  setLookAndFeel(nullptr);
}

void PluginEditor::resized() {
  auto bounds = getLocalBounds();

  background.setBounds(bounds);

  bounds.removeFromTop(28);
  headerLabel.setBounds(bounds.removeFromTop(18));

  bounds.removeFromTop(12);

  constexpr auto widgetMargin = 26;
  bounds.removeFromLeft(widgetMargin);
  bounds.removeFromRight(widgetMargin);

  auto labelsBounds = bounds.removeFromTop(18);
  const auto oneThirdOfWidth = labelsBounds.getWidth() / 3;
  waveformLabel.setBounds(labelsBounds.removeFromLeft(oneThirdOfWidth));
  bypassLabel.setBounds(labelsBounds.removeFromRight(oneThirdOfWidth));
  rateLabel.setBounds(labelsBounds);

  auto widgetBounds = bounds.removeFromTop(67);

  waveformComboBox.setBounds(
      widgetBounds.removeFromLeft(oneThirdOfWidth).reduced(0, 15));

  auto bypassButtonBounds =
      widgetBounds.removeFromRight(oneThirdOfWidth).reduced(8, 14);
  bypassButton.setBounds(bypassButtonBounds);

  rateSlider.setBounds(widgetBounds);

  bounds.removeFromTop(12);
  bounds.removeFromBottom(16);

  lfoVisualizer.setBounds(bounds);
}
}  // namespace ws
