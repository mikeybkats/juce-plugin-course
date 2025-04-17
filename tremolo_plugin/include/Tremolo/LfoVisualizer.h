#pragma once

namespace ws {
class LfoVisualizer : public juce::Component {
public:
  using ReadAllLfoSamples = std::function<void(juce::AudioBuffer<float>&)>;
  using GetCurrentSampleRate = std::function<double()>;

  LfoVisualizer(ReadAllLfoSamples readSamples, GetCurrentSampleRate getRate)
      : readAllLfoSamples{readSamples}, getCurrentSampleRate{getRate} {
    lfoSamplesToPlot.resize(pointsOnPath, 0.f);
    samplesToPath();
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(backgroundColour);

    g.setColour(curveColor);
    g.strokePath(
        lfoCurve,
        juce::PathStrokeType{4.f, juce::PathStrokeType::JointStyle::curved},
        getLfoCurveTransform());
  }

  void setCurveColor(const juce::Colour& c) { curveColor = c; }

  void setBackgroundColor(const juce::Colour& c) { backgroundColour = c; }

private:
  static constexpr auto pointsOnPath = 22050u;
  static constexpr auto periodsToPlotOf1HzWaveform = 4u;

  void update(double timestampSeconds) {
    updateLfoCurve(timestampSeconds);
    repaint();
  }

  void updateLfoCurve(double timestampSeconds) {
    updateSamplesQueue(timestampSeconds);
    samplesToPath();
  }

  void updateSamplesQueue(double timestampSeconds) {
    if (!lastTimestampSeconds.has_value()) {
      lastTimestampSeconds = timestampSeconds;
      return;
    }

    readAllLfoSamples(buffer);

    const auto stride = getStride();

    const auto newAvailableSamples = buffer.getNumSamples();
    if (newAvailableSamples > 0) {
      for (; sampleIndex < newAvailableSamples; sampleIndex += stride) {
        const auto sample = buffer.getSample(0, sampleIndex);
        lfoSamplesToPlot.pop_front();
        lfoSamplesToPlot.push_back(sample);
      }
      buffer.clear();
    } else {
      const auto secondsPassed =
          timestampSeconds - lastTimestampSeconds.value();
      const auto samplesPassed =
          static_cast<int>(getCurrentSampleRate() * secondsPassed);
      for (; sampleIndex < samplesPassed; sampleIndex += stride) {
        lfoSamplesToPlot.pop_front();
        lfoSamplesToPlot.push_back(0.f);
      }
    }
    sampleIndex %= stride;

    lastTimestampSeconds = timestampSeconds;
  }

  int getStride() const {
    return static_cast<int>(getCurrentSampleRate() *
                            periodsToPlotOf1HzWaveform / pointsOnPath);
  }

  void samplesToPath() {
    juce::Path path;
    path.startNewSubPath(0.f, lfoSamplesToPlot.front());
    for (const auto i : std::views::iota(1u, lfoSamplesToPlot.size())) {
      path.lineTo(static_cast<float>(i), lfoSamplesToPlot.at(i));
    }
    lfoCurve = path;
  }

  /** @brief Creates a transform that maps current LFO curve to component bounds
   *
   * @detail The transform is based on following point mappings:
   *
   *   (0,ylim)                        -> (0,0) (left-top corner)
   *   (0,-ylim)                       -> (0, height) (left-bottom corner)
   *   (curve end X coordinate, -ylim) -> (component width, component height)
   *                                      (right-bottom corner)
   */
  juce::AffineTransform getLfoCurveTransform() const {
    constexpr auto ylim = 1.1f;
    const auto bounds = getLocalBounds().toFloat();
    const auto transform = juce::AffineTransform::fromTargetPoints(
        0.f, ylim, 0.f, 0.f, 0.f, -ylim, 0.f, bounds.getHeight(),
        lfoCurve.getCurrentPosition().getX(), -ylim, bounds.getWidth(),
        bounds.getHeight());
    return transform;
  }

  juce::Colour curveColor{juce::Colours::black};
  juce::Colour backgroundColour{juce::Colours::white};
  ReadAllLfoSamples readAllLfoSamples;
  GetCurrentSampleRate getCurrentSampleRate;
  juce::AudioBuffer<float> buffer;
  juce::Path lfoCurve;
  std::deque<float> lfoSamplesToPlot;
  int sampleIndex{0};

  std::optional<double> lastTimestampSeconds;
  juce::VBlankAttachment vblankAttachment{
      this, [this](double timestampSeconds) { update(timestampSeconds); }};
};
}  // namespace ws
