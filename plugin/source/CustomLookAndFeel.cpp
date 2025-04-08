#include "Tremolo/CustomLookAndFeel.h"

namespace ws {
CustomLookAndFeel::CustomLookAndFeel() {
  setColour(juce::TextButton::buttonOnColourId,
            getColour<CustomLookAndFeel::Colours::ORANGE>());
}
}  // namespace ws
