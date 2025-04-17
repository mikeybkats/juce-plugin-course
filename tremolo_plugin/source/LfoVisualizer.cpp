
namespace ws {

LfoVisualizer::LfoVisualizer(ReadAllLfoSamples readSamples,
                             GetCurrentSampleRate getRate)
    : readAllLfoSamples{readSamples}, getCurrentSampleRate{getRate} {
  samplesToPath();
}

void LfoVisualizer::paint(juce::Graphics& g) {
  g.fillAll(backgroundColour);

  g.setColour(curveColor);
  g.strokePath(
      lfoCurve,
      juce::PathStrokeType{4.f, juce::PathStrokeType::JointStyle::curved},
      getLfoCurveTransform());
}

void LfoVisualizer::setCurveColor(juce::Colour c) {
  curveColor = c;
}

void LfoVisualizer::setBackgroundColor(juce::Colour c) {
  backgroundColour = c;
}

void LfoVisualizer::update(double timestampSeconds) {
  updateLfoCurve(timestampSeconds);
  repaint();
}

void LfoVisualizer::updateLfoCurve(double timestampSeconds) {
  updateSamplesQueue(timestampSeconds);
  samplesToPath();
}

void LfoVisualizer::updateSamplesQueue(double timestampSeconds) {
  if (!lastTimestampSeconds.has_value()) {
    lastTimestampSeconds = timestampSeconds;
    return;
  }

  readAllLfoSamples(buffer);

  lfoSamplesToPlot.setStride(getStride());

  const auto newAvailableSamples = buffer.getNumSamples();
  if (newAvailableSamples > 0) {
    lfoSamplesToPlot.pushBack(buffer);
    buffer.clear();
  } else {
    const auto secondsPassed = timestampSeconds - lastTimestampSeconds.value();
    const auto samplesPassed =
        static_cast<int>(getCurrentSampleRate() * secondsPassed);
    lfoSamplesToPlot.pushBackZeros(samplesPassed);
  }

  lastTimestampSeconds = timestampSeconds;
}

int LfoVisualizer::getStride() const {
  return static_cast<int>(getCurrentSampleRate() * periodsToPlotOf1HzWaveform /
                          pointsOnPath);
}

void LfoVisualizer::samplesToPath() {
  juce::Path path;
  path.startNewSubPath(0.f, lfoSamplesToPlot.front());
  for (const auto i : std::views::iota(1u, lfoSamplesToPlot.size())) {
    path.lineTo(static_cast<float>(i), lfoSamplesToPlot.at(i));
  }
  lfoCurve = path;
}

juce::AffineTransform LfoVisualizer::getLfoCurveTransform() const {
  constexpr auto ylim = 1.1f;
  const auto bounds = getLocalBounds().toFloat();
  const auto transform = juce::AffineTransform::fromTargetPoints(
      0.f, ylim, 0.f, 0.f, 0.f, -ylim, 0.f, bounds.getHeight(),
      lfoCurve.getCurrentPosition().getX(), -ylim, bounds.getWidth(),
      bounds.getHeight());
  return transform;
}
}  // namespace ws
