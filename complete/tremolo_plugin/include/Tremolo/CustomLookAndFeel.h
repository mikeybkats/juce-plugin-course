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
      juce::Label& label) override;

  juce::Font getPopupMenuFont() override;

  juce::Path getTickShape(float) override;

  void drawToggleButton(juce::Graphics&,
                        juce::ToggleButton&,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;

private:
  class FontContainer {
  public:
    FontContainer();

  private:
    juce::Typeface::Ptr interRegular;
    juce::Typeface::Ptr interBold;
    juce::Typeface::Ptr interMedium;
  };

  void drawButtonInset(juce::Graphics&,
                       const juce::Rectangle<float>& bounds) const;
  void drawPlainButton(juce::Graphics&,
                       const juce::Rectangle<float>& bounds) const;

  juce::Typeface::Ptr interTypeface;
  FontContainer fontContainer;
};
}  // namespace ws
