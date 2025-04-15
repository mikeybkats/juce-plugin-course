#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <type_traits>

namespace ws {
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
  enum class Colors : size_t { orange, lightGrey };

  CustomLookAndFeel();

  void drawComboBox(juce::Graphics&,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox&) override;

  static juce::Colour getColor(Colors colorName);
};
}  // namespace ws
