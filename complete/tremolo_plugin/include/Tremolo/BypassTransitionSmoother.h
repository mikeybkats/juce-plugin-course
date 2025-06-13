#pragma once

namespace tremolo {
template <typename FloatType>
class FixedStepRangedSmoothedValue
    : public juce::SmoothedValueBase<FixedStepRangedSmoothedValue<FloatType>> {
public:
  FixedStepRangedSmoothedValue(FloatType minValue, FloatType maxValue)
      : a{minValue}, b{maxValue}, range{minValue, maxValue} {
    jassert(minValue < maxValue);
    jassert(!juce::approximatelyEqual(minValue, maxValue));

    this->currentValue = minValue;
    this->target = minValue;
  }

  void reset(double sampleRate, double rampLengthSeconds) {
    const auto rampLengthSamples =
        static_cast<int>(std::floor(rampLengthSeconds * sampleRate));
    step = range.getLength() / static_cast<FloatType>(rampLengthSamples);
  }

  void setTargetValue(bool goToMax) {
    this->target = goToMax ? range.getEnd() : range.getStart();
    step =
        (this->currentValue < this->target) ? std::abs(step) : -std::abs(step);
    this->countdown =
        static_cast<int>(std::abs((this->currentValue - this->target) / step));
  }

  FloatType getNextValue() {
    if (!this->isSmoothing()) {
      return this->target;
    }

    --(this->countdown);

    if (this->isSmoothing()) {
      this->currentValue += step;
    } else {
      this->currentValue = this->target;
    }

    return this->currentValue;
  }

private:
  FloatType a;
  FloatType b;
  juce::Range<FloatType> range;
  FloatType step{0.f};
};

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
    dryGain.setCurrentAndTargetValue(0.f);
    wetGain.setCurrentAndTargetValue(1.f);
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
    if (bypass == isBypassed) {
      return;
    }

    dryGain.setTargetValue(bypass);
    wetGain.setTargetValue(!bypass);

    isBypassed = bypass;
  }

  [[nodiscard]] bool isTransitioning() const noexcept {
    return dryGain.isSmoothing() || wetGain.isSmoothing();
  }

  void setDryBuffer(const juce::AudioBuffer<float>& buffer) noexcept {
    if (!isTransitioning()) {
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
    if (!isTransitioning()) {
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
    isBypassed = false;
    dryBuffer.clear();
  }

private:
  double crossfadeLengthSeconds;
  bool isBypassed = false;
  juce::AudioBuffer<float> dryBuffer;
  FixedStepRangedSmoothedValue<float> dryGain{0.f, 1.f};
  FixedStepRangedSmoothedValue<float> wetGain{0.f, 1.f};
};
}  // namespace tremolo
