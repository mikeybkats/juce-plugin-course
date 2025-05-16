namespace ws {
CustomLookAndFeel::CustomLookAndFeel() {
  setColour(juce::TextButton::buttonOnColourId, getColor(Colors::orange));
  setColour(juce::TextButton::buttonColourId, getColor(Colors::lightGrey));
  setColour(juce::ComboBox::backgroundColourId, getColor(Colors::lightGrey));
  setColour(juce::ComboBox::outlineColourId,
            getColor(Colors::lightGrey).darker());
  setColour(juce::ComboBox::arrowColourId,
            getColor(Colors::lightGrey).darker());
  setColour(juce::ComboBox::textColourId, getColor(Colors::white));
  setColour(juce::Label::textColourId, getColor(Colors::white));
  setColour(juce::ToggleButton::textColourId, getColor(Colors::white));
  setColour(juce::ToggleButton::tickColourId, getColor(Colors::white));
  setColour(juce::ToggleButton::tickDisabledColourId,
            juce::Colours::black.brighter());

  setColour(juce::PopupMenu::backgroundColourId, juce::Colour{0xFF153245});
  setColour(juce::PopupMenu::textColourId, getColor(Colors::white));
  setColour(juce::PopupMenu::highlightedTextColourId, juce::Colour{0xFF0C131E});
  setColour(juce::PopupMenu::highlightedBackgroundColourId,
            getColor(Colors::orange));

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

  auto insetGradient = juce::ColourGradient::vertical(
      juce::Colour{0xFF22232C}, 0.f, juce::Colour{0xFF263235},
      boxBounds.toFloat().getHeight());
  insetGradient.addColour(0.35, juce::Colour{0xFF303538});
  g.setGradientFill(insetGradient);
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

  const auto buttonBounds = boxBounds.toFloat().reduced(2.f);
  auto buttonGradient = juce::ColourGradient::vertical(
      juce::Colour{0xFF4A7090}, buttonBounds.getY(), juce::Colour{0xFF324258},
      buttonBounds.getBottom());
  buttonGradient.addColour(0.73, juce::Colour{0xFF315160});
  g.setGradientFill(buttonGradient);
  g.fillRoundedRectangle(buttonBounds, 4);

  auto arrowZone = boxBounds.toFloat().reduced(10.f, 11.f);
  arrowZone.removeFromLeft(104);
  juce::Path path;
  path.startNewSubPath(arrowZone.getTopLeft());
  path.lineTo(arrowZone.getCentreX(), arrowZone.getBottom());
  path.lineTo(arrowZone.getTopRight());

  g.setColour(
      getColor(Colors::white).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
  g.fillPath(path);
}

juce::Colour CustomLookAndFeel::getColor(Colors colorName) {
  static const std::array colors{
      juce::Colour{0xFFFFAA00},
      juce::Colour{0xFFD9D9D9},
      juce::Colour{0xFFDDECFF},
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

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox&) {
  return juce::FontOptions{}.withPointHeight(12.f).withStyle("Medium");
}

void CustomLookAndFeel::positionComboBoxText(juce::ComboBox& comboBox,
                                             juce::Label& labelToPosition) {
  auto bounds = comboBox.getLocalBounds().reduced(10, 6);
  bounds.removeFromRight(12);
  labelToPosition.setBounds(bounds);
}

juce::PopupMenu::Options CustomLookAndFeel::getOptionsForComboBoxPopupMenu(
    juce::ComboBox& box,
    juce::Label& label) {
  const auto screenBounds = box.getScreenBounds();
  auto menuBounds = screenBounds.reduced(2, 0);
  constexpr auto itemHeight = 24;
  return juce::LookAndFeel_V4::getOptionsForComboBoxPopupMenu(box, label)
      .withStandardItemHeight(itemHeight)
      .withTargetScreenArea(menuBounds)
      .withMinimumWidth(128);
}

juce::Font CustomLookAndFeel::getPopupMenuFont() {
  return juce::FontOptions{}.withStyle("Medium").withPointHeight(12.f);
}

juce::Path CustomLookAndFeel::getTickShape(float) {
  return {};
}
}  // namespace ws
