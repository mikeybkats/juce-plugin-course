#pragma once

namespace tremolo {
class AboutComponent : public juce::Component {
public:
  AboutComponent() {
    popup.setAllowedPlacement(juce::BubbleComponent::BubblePlacement::below);
    popup.setAlwaysOnTop(true);
    popup.addToDesktop(0);

    text.setColour(juce::Colours::orange);
    text.setJustification(juce::Justification::centred);
  }

  void paint(juce::Graphics& g) override {
    g.setColour(juce::Colour{0xFF315160});
    constexpr auto thickness = 2.f;
    g.drawEllipse(getLocalBounds().toFloat().reduced(thickness), thickness);
    g.setFont(CustomLookAndFeel::getSideLabelsFont());
    g.drawFittedText("i", getLocalBounds(), juce::Justification::centred, 1);
  }

  void mouseEnter(const juce::MouseEvent& event) override {
    popup.showAt(this, text, 0, false);
  }

  void mouseExit(const juce::MouseEvent& event) override {
    popup.setVisible(false);
  }

private:
  juce::BubbleMessageComponent popup;
  juce::AttributedString text{"message"};
};
}  // namespace tremolo
