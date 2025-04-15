#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <type_traits>

namespace ws {
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
private:
  const auto& getColorPalette() {
    static const std::array colours{
        juce::Colour{0xFFEF7600},
        juce::Colour{0xFFD9D9D9},
    };
    return colours;
  }

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

  template <Colors ColorName>
  const juce::Colour& getColor() {
    return std::get<static_cast<std::underlying_type_t<Colors>>(ColorName)>(
        getColorPalette());
  }
};
}  // namespace ws
