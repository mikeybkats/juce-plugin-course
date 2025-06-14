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
  explicit BypassTransitionSmoother(double crossfadeLengthSecondsValue = 0.01)
      : crossfadeLengthSeconds{crossfadeLengthSecondsValue} {
    jassert(0.0 <= crossfadeLengthSeconds);

    reset();
  }

  void prepare(double sampleRate,
               int channelCount,
               int expectedMaxFramesPerBlock) {
    dryBuffer.setSize(channelCount, expectedMaxFramesPerBlock);
    dryGain.reset(sampleRate, crossfadeLengthSeconds);
    wetGain.reset(sampleRate, crossfadeLengthSeconds);
    reset();
  }

  void setBypass(bool bypass) noexcept {
    dryGain.setTargetValueToExtreme(bypass);
    wetGain.setTargetValueToExtreme(!bypass);
  }

  void setBypassForced(bool bypass) noexcept {
    dryGain.setCurrentAndTargetValueToExtreme(bypass);
    wetGain.setCurrentAndTargetValueToExtreme(!bypass);
  }

  [[nodiscard]] bool isTransitioning() const noexcept {
    // no need to check wetGain; both gains move in tandem
    return dryGain.isSmoothing();
  }

  void setDryBuffer(const juce::AudioBuffer<float>& buffer) noexcept {
    if (!isTransitioning() && !isBypassed()) {
      // plugin is operational: no need to store the dry buffer
      return;
    }

    jassert(buffer.getNumSamples() <= dryBuffer.getNumSamples());
    jassert(buffer.getNumChannels() <= dryBuffer.getNumChannels());

    for (const auto channel : std::views::iota(0, buffer.getNumChannels())) {
      dryBuffer.copyFrom(channel, 0, buffer, channel, 0,
                         buffer.getNumSamples());
    }
    dryGain.applyGain(dryBuffer, buffer.getNumSamples());
  }

  void mixToWetBuffer(juce::AudioBuffer<float>& buffer) noexcept {
    if (!isTransitioning() && !isBypassed()) {
      // plugin is operational: no need to modify the wet buffer
      return;
    }

    jassert(buffer.getNumSamples() <= dryBuffer.getNumSamples());
    jassert(buffer.getNumChannels() <= dryBuffer.getNumChannels());

    wetGain.applyGain(buffer, buffer.getNumSamples());
    for (const auto channel : std::views::iota(0, buffer.getNumChannels())) {
      buffer.addFrom(channel, 0, dryBuffer, channel, 0, buffer.getNumSamples());
    }
  }

  void reset() noexcept {
    setBypassForced(false);
    dryBuffer.clear();
  }

private:
  [[nodiscard]] bool isBypassed() const noexcept {
    // no need to check wetGain; both gains move in tandem
    return juce::approximatelyEqual(dryGain.getTargetValue(),
                                    dryGain.getRange().getEnd());
  }

  double crossfadeLengthSeconds = 0.0;
  juce::AudioBuffer<float> dryBuffer;
  FixedStepRangedSmoothedValue<float> dryGain{{0.f, 1.f}};
  FixedStepRangedSmoothedValue<float> wetGain{{0.f, 1.f}};
};
}  // namespace tremolo
