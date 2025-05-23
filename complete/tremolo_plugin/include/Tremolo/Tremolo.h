#pragma once

namespace ws {
class Tremolo {
public:
  enum class LfoWaveform : size_t {
    sine = 0,
    triangle = 1,
  };

  explicit Tremolo() {
    std::ranges::for_each(
        lfos, [](auto& lfo) { lfo.setFrequency(5.f /* Hz */, true); });
  }

  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    std::ranges::for_each(
        lfos, [spec = juce::dsp::ProcessSpec{
                   .sampleRate = sampleRate,
                   .maximumBlockSize =
                       static_cast<juce::uint32>(expectedMaxFramesPerBlock),
                   .numChannels = 1u,
               }](auto& lfo) { lfo.prepare(spec); });
    lfoSampleFifo.prepare(sampleRate);
    lfoTransitionSmoother.reset(sampleRate, 0.025 /* 25 milliseconds */);

    // allocate defensively
    lfoSamples.resize(4u * static_cast<size_t>(expectedMaxFramesPerBlock));
  }

  void setModulationRate(float rateHz) noexcept {
    std::ranges::for_each(lfos,
                          [rateHz](auto& lfo) { lfo.setFrequency(rateHz); });
  }

  void setLfoWaveform(LfoWaveform waveform) {
    jassert(waveform == LfoWaveform::sine || waveform == LfoWaveform::triangle);

    lfoToSet = waveform;
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // actual updating of the LFO waveform happens in process()
    // to keep setLfoWaveform() idempotent
    updateLfoWaveform();

    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      // generate the LFO value
      const auto lfoValue = getNextLfoValue();
      lfoSampleFifo.push(lfoValue);

      // calculate the modulation value
      const auto modulationValue = modulationDepth * lfoValue + 1.f;

      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // modulate the sample
        const auto outputSample = modulationValue * inputSample;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
    }
  }

  void processChannelwise(juce::AudioBuffer<float>& buffer) noexcept {
    // actual updating of the LFO waveform happens in process()
    // to keep setLfoWaveform() idempotent
    updateLfoWaveform();

    const auto samplesToProcess = std::min(
        lfoSamples.size(), static_cast<size_t>(buffer.getNumSamples()));

    // detect if the host is misbehaving; if this fails, then many more frames
    // have been given for processing than declared in prepare()
    jassert(samplesToProcess <= lfoSamples.size());

    // generate LFO signal
    for (const auto i : std::views::iota(0u, samplesToProcess)) {
      lfoSamples[i] = getNextLfoValue();
      lfoSampleFifo.push(lfoSamples[i]);
    }

    // calculate the modulation value
    juce::FloatVectorOperations::multiply(lfoSamples.data(), modulationDepth,
                                          samplesToProcess);
    juce::FloatVectorOperations::add(lfoSamples.data(), 1.f, samplesToProcess);

    // for each channel
    for (const auto channelIndex :
         std::views::iota(0, buffer.getNumChannels())) {
      juce::FloatVectorOperations::multiply(
          buffer.getWritePointer(channelIndex), lfoSamples.data(),
          samplesToProcess);
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
  static constexpr auto modulationDepth = 0.1f;

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
    // to get just the generated sample
    return lfos[juce::toUnderlyingType(currentLfo)].processSample(0.f);
  }

  std::array<juce::dsp::Oscillator<float>, 2u> lfos{
      juce::dsp::Oscillator<float>{[](auto phase) { return std::sin(phase); }},
      juce::dsp::Oscillator<float>{triangle}};

  LfoWaveform currentLfo = LfoWaveform::sine;
  LfoWaveform lfoToSet = currentLfo;

  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
      lfoTransitionSmoother{0.f};
  std::vector<float> lfoSamples;

  SampleFifo<float> lfoSampleFifo;
};
}  // namespace ws
