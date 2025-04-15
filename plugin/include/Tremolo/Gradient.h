#pragma once
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ws {
struct GradientComponent : public juce::Component {
  juce::Colour topColour;
  juce::Colour bottomColour;

  void paint(juce::Graphics& g) override {
    g.setGradientFill(juce::ColourGradient::vertical(topColour, bottomColour,
                                                     getLocalBounds()));
    g.fillAll();
  }
};
}  // namespace ws
