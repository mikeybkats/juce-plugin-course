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
    const auto initialSize = static_cast<size_t>(getCurrentSampleRate());
    lfoSamples.resize(initialSize);
    std::fill_n(lfoSamples.begin(), initialSize, 0.f);
    decimateSamplesToPath();
    startTimer(updateIntervalMs);
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(backgroundColour);

    g.setColour(juce::Colour{0xFFEF7600});
    g.strokePath(lfoCurve, juce::PathStrokeType{4.f}, getLfoCurveTransform());
  }

private:
  static constexpr auto updateIntervalMs = 50;

  void timerCallback() override {
    updateLfoCurve();
    repaint();
  }

  void updateLfoCurve() {
    updateSamplesQueue();
    decimateSamplesToPath();
  }

  void updateSamplesQueue() {
    readAllLfoSamples(buffer);

    const auto newAvailableSamples = buffer.getNumSamples();
    if (newAvailableSamples > 0) {
      std::for_each_n(buffer.getReadPointer(0), newAvailableSamples,
                      [this](float sample) {
                        lfoSamples.pop_front();
                        lfoSamples.push_back(sample);
                      });
      buffer.clear();
    } else {
      const auto zerosToFill =
          static_cast<int>(getCurrentSampleRate() * updateIntervalMs / 1000.0);
      for ([[maybe_unused]] const auto i : std::views::iota(0, zerosToFill)) {
        lfoSamples.pop_front();
        lfoSamples.push_back(0.f);
      }
    }
  }

  void decimateSamplesToPath() {
    constexpr auto pointsOnPath = 400u;
    const auto stride = static_cast<size_t>(lfoSamples.size() / pointsOnPath);

    juce::Path path;
    path.startNewSubPath(0.f, lfoSamples.front());
    for (auto i = stride; i < lfoSamples.size(); i += stride) {
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
    const auto bounds = getLocalBounds().toFloat();
    const auto transform = juce::AffineTransform::fromTargetPoints(
        0.f, 1.f, 0.f, 0.f, 0.f, -1.f, 0.f, bounds.getHeight(),
        lfoCurve.getCurrentPosition().getX(), -1.f, bounds.getWidth(),
        bounds.getHeight());
    return transform;
  }

  juce::Colour backgroundColour{0xFFD9D9D9};
  ReadAllLfoSamples readAllLfoSamples;
  GetCurrentSampleRate getCurrentSampleRate;
  juce::AudioBuffer<float> buffer;
  juce::Path lfoCurve;
  std::deque<float> lfoSamples;
};
}  // namespace ws
