#pragma once

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ws {
class BypassButton : public juce::TextButton {
public:
  BypassButton() { setClickingTogglesState(true); }

  void paintButton(juce::Graphics& g,
                   bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    if (shouldDrawButtonAsDown || !getToggleState()) {
      setButtonText("Bypassed");
    } else {
      setButtonText("On");
    }
    juce::TextButton::paintButton(g, shouldDrawButtonAsHighlighted,
                                  shouldDrawButtonAsDown);
  }
};
}  // namespace ws
