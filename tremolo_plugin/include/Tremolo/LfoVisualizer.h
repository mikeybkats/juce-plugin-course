#pragma once

namespace ws {
class LfoVisualizer : public juce::Component {
public:
  using ReadAllLfoSamples = std::function<void(juce::AudioBuffer<float>&)>;
  using GetCurrentSampleRate = std::function<double()>;

  LfoVisualizer(ReadAllLfoSamples readSamples, GetCurrentSampleRate getRate);

  void paint(juce::Graphics& g) override;

  void setCurveColor(juce::Colour c);

  void setBackgroundColor(juce::Colour c);

private:
  static constexpr auto pointsOnPath = 22050u;
  static constexpr auto periodsToPlotOf1HzWaveform = 4u;

  void update(double timestampSeconds);

  void updateLfoCurve(double timestampSeconds);

  void updateSamplesQueue(double timestampSeconds);

  int getStride() const;

  void samplesToPath();

  /** @brief Creates a transform that maps current LFO curve to component bounds
   *
   * @detail The transform is based on following point mappings:
   *
   *   (0,ylim)                        -> (0,0) (left-top corner)
   *   (0,-ylim)                       -> (0, height) (left-bottom corner)
   *   (curve end X coordinate, -ylim) -> (component width, component height)
   *                                      (right-bottom corner)
   */
  juce::AffineTransform getLfoCurveTransform() const;

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
