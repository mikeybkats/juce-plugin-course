#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ws {
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
private:
  const auto& getColorPalette() {
    static const std::array COLOURS{
        juce::Colour{0xFFEF7600},
        juce::Colour{0xFFD9D9D9},
    };
    return COLOURS;
  }

public:
  enum Colours : size_t { ORANGE, LIGHT_GREY };

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

private:
  template <Colours ColourName>
  const juce::Colour& getColor() {
    return std::get<ColourName>(getColorPalette());
  }
};
}  // namespace ws
