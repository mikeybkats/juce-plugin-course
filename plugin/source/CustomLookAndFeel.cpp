#include "Tremolo/CustomLookAndFeel.h"
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace ws {
CustomLookAndFeel::CustomLookAndFeel() {
  setColour(juce::TextButton::buttonOnColourId, getColor(Colors::orange));
  setColour(juce::TextButton::buttonColourId, getColor(Colors::lightGrey));
  setColour(juce::ComboBox::backgroundColourId, getColor(Colors::lightGrey));
  setColour(juce::ComboBox::outlineColourId,
            getColor(Colors::lightGrey).darker());
  setColour(juce::ComboBox::arrowColourId,
            getColor(Colors::lightGrey).darker());
  setColour(juce::ComboBox::textColourId, juce::Colours::black);
  setColour(juce::Label::textColourId, juce::Colours::black);
  setColour(juce::ToggleButton::textColourId, juce::Colours::black);
  setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
  setColour(juce::ToggleButton::tickDisabledColourId,
            juce::Colours::black.brighter());

  setColour(juce::PopupMenu::backgroundColourId, getColor(Colors::lightGrey));
  setColour(juce::PopupMenu::textColourId, juce::Colours::black);
  setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);
  setColour(juce::PopupMenu::highlightedBackgroundColourId,
            getColor(Colors::orange));

  setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g,
                                     int width,
                                     int height,
                                     bool /* isButtonDown */,
                                     int /* buttonX */,
                                     int /* buttonY */,
                                     int /* buttonW */,
                                     int /* buttonH */,
                                     juce::ComboBox& box) {
  constexpr auto cornerSize = 6.f;
  juce::Rectangle<int> boxBounds(0, 0, width, height);

  g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

  g.setColour(box.findColour(juce::ComboBox::outlineColourId));
  g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize,
                         1.0f);

  auto arrowZone =
      juce::Rectangle<int>(width - height, 0, height, height).toFloat();
  g.drawRoundedRectangle(arrowZone, cornerSize, 1.f);

  arrowZone.reduce((arrowZone.getWidth() - 20.f) / 2.f, 0.f);
  juce::Path path;
  path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
  path.lineTo(arrowZone.getCentreX(), arrowZone.getCentreY() + 3.0f);
  path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

  g.setColour(box.findColour(juce::ComboBox::arrowColourId)
                  .withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
  g.strokePath(path, juce::PathStrokeType(2.0f));
}

juce::Colour CustomLookAndFeel::getColor(Colors colorName) {
  static const std::array colors{
      juce::Colour{0xFFEF7600},
      juce::Colour{0xFFD9D9D9},
  };
  return colors.at(static_cast<std::underlying_type_t<Colors>>(colorName));
}
}  // namespace ws
