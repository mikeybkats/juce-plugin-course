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

  interTypeface = juce::Typeface::createSystemTypefaceFor(
      assets::InterVariableFont_opszwght_ttf,
      assets::InterVariableFont_opszwght_ttfSize);
  getDefaultLookAndFeel().setDefaultSansSerifTypeface(interTypeface);
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
      juce::Colour{0xFFFFAA00},
      juce::Colour{0xFFD9D9D9},
  };
  return colors.at(static_cast<std::underlying_type_t<Colors>>(colorName));
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                         int x,
                                         int y,
                                         int width,
                                         int height,
                                         float sliderPos,
                                         const float rotaryStartAngle,
                                         const float rotaryEndAngle,
                                         juce::Slider& slider) {
  const auto knobCanalColor = juce::Colour{0xFF2A3A3B};

  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(3.75f);

  g.setColour(knobCanalColor);
  g.fillEllipse(bounds);

  bounds.reduce(0.25f, 0.25f);

  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
  auto toAngle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  auto lineW = juce::jmin(8.0f, radius * 0.5f);
  auto arcRadius = radius - lineW * 0.5f;

  if (slider.isEnabled()) {
    juce::Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius,
                           arcRadius, 0.0f, rotaryStartAngle, toAngle, true);

    g.setColour(getColor(Colors::orange));
    g.strokePath(valueArc,
                 juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                      juce::PathStrokeType::square));
  }

  const auto knobBounds = bounds.reduced(4.f);

  auto knobFill = juce::ColourGradient::vertical(
      juce::Colour{0xFF4A7090}, knobBounds.getY(), juce::Colour{0xFF060F1C},
      knobBounds.getY() + knobBounds.getHeight());
  knobFill.addColour(0.29, juce::Colour{0xFF396086});
  knobFill.addColour(0.75, juce::Colour{0xFF2C3648});
  g.setGradientFill(knobFill);
  g.fillEllipse(knobBounds);

  // Knob stroke
  g.setColour(juce::Colour{0x400B1E3A});
  constexpr auto knobStrokeThickness = 1.33f;
  g.drawEllipse(knobBounds.reduced(knobStrokeThickness / 2.f),
                knobStrokeThickness);

  // Knob top
  const auto knobTopBounds = knobBounds.reduced(7.f);
  auto knobTopFill = juce::ColourGradient{juce::Colour{0xFF6697CB},
                                          knobTopBounds.getCentreX(),
                                          knobTopBounds.getY() - 7.f,
                                          juce::Colour{0xFF1B1E48},
                                          knobTopBounds.getCentreX(),
                                          knobTopBounds.getBottom() + 41.f,
                                          true};
  knobTopFill.addColour(0.66, juce::Colour{0xFF0C2338});
  g.setGradientFill(knobTopFill);
  g.fillEllipse(knobTopBounds);

  // TODO: Add knob top blur

  // Knob top edge
  auto knobTopEdgeFill = juce::ColourGradient{juce::Colour{0xFF8FFFFF},
                                              knobTopBounds.getCentreX(),
                                              knobTopBounds.getY(),
                                              juce::Colour{0xFF1A0F4E},
                                              knobTopBounds.getCentreX(),
                                              knobTopBounds.getBottom() + 6.f,
                                              true};
  knobTopEdgeFill.addColour(0.55, juce::Colour{0xFF8078F4});
  g.setGradientFill(knobTopEdgeFill);
  g.setOpacity(0.1f);
  g.drawEllipse(knobTopBounds, 1.f);
}
}  // namespace ws
