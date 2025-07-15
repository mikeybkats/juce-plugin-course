#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent : public juce::Component {
public:
  MainComponent() { setBounds(0, 0, 400, 300); }

  void paint(juce::Graphics& g) override {
    g.setColour(juce::Colours::red);
    g.fillRect(rect1);
    g.setColour(juce::Colours::blue);
    g.fillRect(rect2);
  }

  void resized() override {
    const auto bounds = getLocalBounds().toFloat();
    const auto halfHeight = bounds.getHeight() / 2.f;

    rect1 = juce::Rectangle<float>{bounds.getX(), bounds.getY(),
                                   bounds.getWidth(), halfHeight};
    rect2 = juce::Rectangle<float>{bounds.getX(), bounds.getY() + halfHeight,
                                   bounds.getWidth(), halfHeight};
  }

private:
  juce::Rectangle<float> rect1;
  juce::Rectangle<float> rect2;
};
