#pragma once
#include <algorithm>
#include <functional>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>

namespace ws {
class LfoVisualizer : public juce::Component, private juce::Timer {
public:
  using ReadAllLfoSamples = std::function<void(juce::AudioBuffer<float>&)>;

  explicit LfoVisualizer(ReadAllLfoSamples callback)
      : readAllLfoSamples{callback} {
    constexpr auto initialSize = 48000;
    lfoSamples.resize(initialSize);
    std::fill_n(lfoSamples.begin(), initialSize, 0.f);
    startTimer(50);
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(backgroundColour);

    g.setColour(juce::Colour{0xFFEF7600});
    const auto bounds = getLocalBounds().toFloat();
    g.strokePath(lfoCurve, juce::PathStrokeType{4.f},
                 lfoCurve.getTransformToScaleToFit(0.f, 0.f, bounds.getWidth(),
                                                   bounds.getHeight(), false));
  }

  void timerCallback() override {
    updateLfoCurve();
    repaint();
  }

  void updateLfoCurve() {
    readAllLfoSamples(buffer);

    const auto newAvailableSamples = buffer.getNumSamples();
    if (newAvailableSamples > 0) {
      std::for_each_n(buffer.getReadPointer(0), newAvailableSamples,
                      [this](float sample) {
                        lfoSamples.pop_front();
                        lfoSamples.push_back(sample);
                      });
      buffer.clear();
    }

    juce::Path path;
    path.startNewSubPath(0.f, lfoSamples.front());
    constexpr auto stride = 100u;
    for (auto i = stride; i < lfoSamples.size(); i += stride) {
      path.lineTo(static_cast<float>(i), lfoSamples.at(i));
    }
    lfoCurve = path;
  }

private:
  juce::Colour backgroundColour{0xFFD9D9D9};
  ReadAllLfoSamples readAllLfoSamples;
  juce::AudioBuffer<float> buffer;
  juce::Path lfoCurve;
  std::deque<float> lfoSamples;
};
}  // namespace ws
