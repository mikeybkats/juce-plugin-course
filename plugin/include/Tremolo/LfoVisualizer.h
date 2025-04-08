#pragma once
#include <algorithm>
#include <functional>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <ranges>

namespace ws {
class LfoVisualizer : public juce::Component, private juce::Timer {
public:
  using ReadAllLfoSamples = std::function<void(juce::AudioBuffer<float>&)>;
  using GetCurrentSampleRate = std::function<double()>;

  LfoVisualizer(ReadAllLfoSamples readSamples, GetCurrentSampleRate getRate)
      : readAllLfoSamples{readSamples}, getCurrentSampleRate{getRate} {
    const auto initialSize = pointsOnPath;
    lfoSamples.resize(initialSize);
    std::fill_n(lfoSamples.begin(), initialSize, 0.f);
    samplesToPath();
    startTimer(updateIntervalMs);
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(backgroundColour);

    g.setColour(juce::Colour{0xFFEF7600});
    g.strokePath(
        lfoCurve,
        juce::PathStrokeType{4.f, juce::PathStrokeType::JointStyle::curved},
        getLfoCurveTransform());
  }

private:
  static constexpr auto pointsOnPath = 800u;
  static constexpr auto updateIntervalMs = 25;
  static constexpr auto periodsToPlotOf1HzWaveform = 4u;

  size_t getNumSamplesToStore() const {
    return static_cast<size_t>(getCurrentSampleRate() *
                               periodsToPlotOf1HzWaveform);
  }

  void timerCallback() override {
    updateLfoCurve();
    repaint();
  }

  void updateLfoCurve() {
    updateSamplesQueue();
    samplesToPath();
  }

  void updateSamplesQueue() {
    readAllLfoSamples(buffer);

    const auto stride = static_cast<int>(
        getCurrentSampleRate() * periodsToPlotOf1HzWaveform / pointsOnPath);

    const auto newAvailableSamples = buffer.getNumSamples();
    if (newAvailableSamples > 0) {
      for (; sampleIndex < newAvailableSamples; sampleIndex += stride) {
        const auto sample = buffer.getSample(0, sampleIndex);
        lfoSamples.pop_front();
        lfoSamples.push_back(sample);
      }
      buffer.clear();
    } else {
      const auto zerosToFill =
          static_cast<int>(getCurrentSampleRate() * updateIntervalMs / 1000.0);
      for (; sampleIndex < zerosToFill; sampleIndex += stride) {
        lfoSamples.pop_front();
        lfoSamples.push_back(0.f);
      }
    }
    while (stride <= sampleIndex) {
      sampleIndex -= stride;
    }
  }

  void samplesToPath() {
    juce::Path path;
    path.startNewSubPath(0.f, lfoSamples.front());
    for (const auto i : std::views::iota(0u, lfoSamples.size())) {
      path.lineTo(static_cast<float>(i), lfoSamples.at(i));
    }
    lfoCurve = path;
  }

  /** @brief Creates a transform that maps current LFO curve to component bounds
   *
   * @detail The transform is based on following point mappings:
   *
   *     (0,1)                        -> (0,0) (left-top corner)
   *     (0,-1)                       -> (0, height) (left-bottom corner)
   *     (curve end X coordinate, -1) -> (component width, component height)
   *                                     (right-bottom corner)
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

  juce::Colour backgroundColour{0xFFD9D9D9};
  ReadAllLfoSamples readAllLfoSamples;
  GetCurrentSampleRate getCurrentSampleRate;
  juce::AudioBuffer<float> buffer;
  juce::Path lfoCurve;
  std::deque<float> lfoSamples;
  int sampleIndex{0};
};
}  // namespace ws
