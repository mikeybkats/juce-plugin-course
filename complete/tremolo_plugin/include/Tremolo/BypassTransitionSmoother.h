#pragma once

namespace tremolo {
/**
 * Class facilitating transition to and from bypassed state over a single block.
 *
 * It allows 2 things:
 *  - detecting that a bypass state transition takes place
 *  - fade-in and fade-out of dry (unprocessed) and wet (processed) buffers
 *    according to the transition type
 *
 * Use the following code in the processBlock() method of your plugin processor:
 *
 * @code
 * //...
 *
 * bypassTransitionSmoother.setBypass(parameters.bypassed);
 *
 * if (bypassTransitionSmoother.isTransitioning()) {
 *   bypassTransitionSmoother.setDryBuffer(buffer);
 *
 *   yourAudioEffectClassInstance.process(buffer);
 *
 *   bypassTransitionSmoother.mixToWetBuffer(buffer);
 *
 *   return;
 * }
 *
 * // avoid processing if bypassed, process otherwise...
 * @endcode
 *
 * Remember to call prepare() in prepareToPlay(),
 * setBypass() in setStateInformation(), and reset() in releaseResources().
 */
class BypassTransitionSmoother {
public:
  void prepare(double sampleRate,
               int channelCount,
               int expectedMaxFramesPerBlock) {
    dryBuffer.setSize(channelCount, expectedMaxFramesPerBlock);
    constexpr auto crossfadeLengthSeconds = 0.1;
    dryGain.reset(sampleRate, crossfadeLengthSeconds);
    wetGain.reset(sampleRate, crossfadeLengthSeconds);
    reset();
  }

  void setBypass(bool bypass) {
    if (bypass == isBypassed) {
      return;
    }

    const auto startDryGain = bypass ? 0.f : 1.f;
    const auto endDryGain = 1.f - startDryGain;

    if (!isTransitioning()) {
      // don't change start gain if previous transition didn't complete
      dryGain.setCurrentAndTargetValue(startDryGain);
    }
    dryGain.setTargetValue(endDryGain);

    const auto startWetGain = bypass ? 1.f : 0.f;
    const auto endWetGain = 1.f - startWetGain;

    if (!isTransitioning()) {
      // don't change start gain if previous transition didn't complete
      wetGain.setCurrentAndTargetValue(startWetGain);
    }
    wetGain.setTargetValue(endWetGain);

    isTransition = true;

    isBypassed = bypass;
  }

  [[nodiscard]] bool isTransitioning() const noexcept { return isTransition; }

  void setDryBuffer(const juce::AudioBuffer<float>& buffer) {
    if (!isTransitioning()) {
      return;
    }

    jassert(buffer.getNumSamples() <= dryBuffer.getNumSamples());
    jassert(buffer.getNumChannels() <= dryBuffer.getNumChannels());

    for (const auto channel : std::views::iota(0, buffer.getNumChannels())) {
      dryBuffer.copyFrom(channel, 0, buffer, channel, 0,
                         buffer.getNumSamples());
    }
    dryGain.applyGain(dryBuffer, dryBuffer.getNumSamples());
  }

  void mixToWetBuffer(juce::AudioBuffer<float>& buffer) {
    if (!isTransitioning()) {
      return;
    }

    jassert(buffer.getNumSamples() <= dryBuffer.getNumSamples());
    jassert(buffer.getNumChannels() <= dryBuffer.getNumChannels());

    wetGain.applyGain(buffer, buffer.getNumSamples());
    for (const auto channel : std::views::iota(0, buffer.getNumChannels())) {
      buffer.addFrom(channel, 0, dryBuffer, channel, 0, buffer.getNumSamples());
    }

    if (!dryGain.isSmoothing() && !wetGain.isSmoothing()) {
      isTransition = false;
    }
  }

  void reset() {
    isBypassed = false;
    isTransition = false;
    dryBuffer.clear();
  }

private:
  bool isBypassed = false;
  bool isTransition = false;
  juce::AudioBuffer<float> dryBuffer;
  juce::SmoothedValue<float> dryGain;
  juce::SmoothedValue<float> wetGain;
};
}  // namespace tremolo