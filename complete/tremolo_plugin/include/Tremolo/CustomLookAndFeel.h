#pragma once

namespace ws {
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
  enum class Colors : size_t { orange, lightGrey, white };

  static juce::Colour getColor(Colors colorName);

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

  void drawRotarySlider(juce::Graphics&,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPosProportional,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider&) override;

  juce::Font getComboBoxFont(juce::ComboBox&) override;

  void positionComboBoxText(juce::ComboBox&,
                            juce::Label& labelToPosition) override;

  juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(
      juce::ComboBox& box,
      juce::Label& label) override {
    const auto screenBounds = box.getScreenBounds();
    auto menuBounds = screenBounds.reduced(2, 0);
    return juce::LookAndFeel_V4::getOptionsForComboBoxPopupMenu(box, label)
        .withStandardItemHeight(24)
        .withTargetScreenArea(menuBounds)
        .withMinimumWidth(128);
  }

  juce::Font getPopupMenuFont() override {
    return juce::FontOptions{}.withStyle("Medium").withPointHeight(12.f);
  }

  juce::Path getTickShape(float) override { return {}; }

private:
  juce::Typeface::Ptr interTypeface;
};
}  // namespace ws
