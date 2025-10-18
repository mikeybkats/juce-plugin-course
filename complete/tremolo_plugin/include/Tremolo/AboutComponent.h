#pragma once

namespace tremolo {
class AboutComponent : public juce::Component {
public:
  AboutComponent() {
    popup.setAllowedPlacement(juce::BubbleComponent::BubblePlacement::below);
    popup.setAlwaysOnTop(true);
    popup.addToDesktop(0);

    text.setColour(
        CustomLookAndFeel::getColor(CustomLookAndFeel::Colors::paleBlue));
    text.setJustification(juce::Justification::centred);
  }

  void paint(juce::Graphics& g) override {
    g.setColour(juce::Colour{0xFF315160});
    constexpr auto thickness = 2.f;
    g.drawEllipse(getLocalBounds().toFloat().reduced(thickness), thickness);
    g.setFont(CustomLookAndFeel::getSideLabelsFont());
    g.drawFittedText("i", getLocalBounds(), juce::Justification::centred, 1);
  }

  void mouseDown(const juce::MouseEvent&) override {
    if (!popup.isVisible()) {
      popup.showAt(this, text, 0, true);
    }
  }

private:
  juce::BubbleMessageComponent popup;
  juce::AttributedString text{JucePlugin_Manufacturer
                              "\n" JucePlugin_Name "\n" __DATE__ " " __TIME__
                              " v" JucePlugin_VersionString};
};
}  // namespace tremolo
