#include "Tremolo/CustomLookAndFeel.h"

namespace ws {
CustomLookAndFeel::CustomLookAndFeel() {
  setColour(juce::TextButton::buttonOnColourId, getColour<Colours::ORANGE>());
  setColour(juce::TextButton::buttonColourId, getColour<Colours::LIGHT_GREY>());
}
}  // namespace ws
