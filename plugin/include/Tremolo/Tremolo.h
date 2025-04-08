#pragma once
#include "SampleFifo.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <ranges>
#include <array>

namespace ws {
class Tremolo {
public:
  enum LfoWaveform : size_t {
    SINE = 0u,
    TRIANGLE = 1u,
    COUNT = 2u,
  };

  Tremolo()
      : lfos{juce::dsp::Oscillator<float>{
                 [](float phase) { return std::sin(phase); }},
             juce::dsp::Oscillator<float>{
                 [](float phase) { return triangle(phase); }}},
        lfoTransitionSmoother{0.f} {
    std::ranges::for_each(lfos, [](auto& lfo) { lfo.setFrequency(5, true); });
  }

  void prepare(double sampleRate) noexcept {
    std::ranges::for_each(lfos, [spec = juce::dsp::ProcessSpec{
                                     .sampleRate = sampleRate,
                                     .maximumBlockSize = 1u,
                                     .numChannels = 1u,
                                 }](auto& lfo) { lfo.prepare(spec); });
    lfoSampleFifo.prepare(sampleRate);
    lfoTransitionSmoother.reset(sampleRate, 0.025 /* 25 milliseconds */);
  }

  void setModulationRate(float rateHz) noexcept {
    std::ranges::for_each(lfos,
                          [rateHz](auto& lfo) { lfo.setFrequency(rateHz); });
  }

  void setLfoWaveform(LfoWaveform waveform) {
    jassert(waveform < LfoWaveform::COUNT);

    if (waveform < LfoWaveform::COUNT) {
      lfoToSet = waveform;
    }
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // actual updating of the LFO waveform happens in process()
    // to keep setLfoWaveform() idempotent
    updateLfoWaveform();

    // for each sample
    for (const auto i : std::views::iota(0, buffer.getNumSamples())) {
      // generate the LFO value
      const auto lfoValue = getNextLfoValue();
      lfoSampleFifo.push(lfoValue);

      // calculate the modulation value
      const auto modulationValue = (1.f + MODULATION_DEPTH * lfoValue);

      for (const auto channel : std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channel, i);

        // modulate the sample
        const auto outputSample = modulationValue * inputSample;

        // set the output sample
        buffer.setSample(channel, i, outputSample);
      }
    }
  }

  void reset() noexcept {
    std::ranges::for_each(lfos, [](auto& lfo) { lfo.reset(); });
    lfoSampleFifo.reset();
  }

  void readAllLfoSamples(juce::AudioBuffer<float>& bufferToFill) {
    lfoSampleFifo.popAll(bufferToFill);
  }

private:
  static constexpr auto MODULATION_DEPTH = 0.1f;

  static float triangle(float phase) {
    // Source:
    // https://thewolfsound.com/sine-saw-square-triangle-pulse-basic-waveforms-in-synthesis/#triangle
    const auto ft = phase / juce::MathConstants<float>::twoPi;
    return 4.f * std::abs(ft - std::floor(ft + 0.5f)) - 1.f;
  }

  void updateLfoWaveform() {
    if (lfoToSet != currentLfo) {
      // update the smoother
      lfoTransitionSmoother.setCurrentAndTargetValue(getNextLfoValue());

      currentLfo = lfoToSet;

      // initiate smoothing
      lfoTransitionSmoother.setTargetValue(getNextLfoValue());
    }
  }

  float getNextLfoValue() {
    if (lfoTransitionSmoother.isSmoothing()) {
      return lfoTransitionSmoother.getNextValue();
    }
    // the argument is added to the generated sample, thus, we pass in 0
    return lfos[currentLfo].processSample(0.f);
  }

  std::array<juce::dsp::Oscillator<float>, LfoWaveform::COUNT> lfos;
  LfoWaveform currentLfo = LfoWaveform::SINE;
  LfoWaveform lfoToSet{currentLfo};
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
      lfoTransitionSmoother;

  SampleFifo<float> lfoSampleFifo;
};
}  // namespace ws
