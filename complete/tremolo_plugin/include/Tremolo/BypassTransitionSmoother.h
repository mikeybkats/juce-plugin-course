#pragma once

namespace tremolo {
template <typename FloatType>
class FixedStepSmoothedValue
    : public juce::SmoothedValueBase<FixedStepSmoothedValue<FloatType>> {
public:
  FixedStepSmoothedValue(FloatType minValue, FloatType maxValue)
      : a{minValue}, b{maxValue} {
    jassert(minValue < maxValue);
    jassert(!juce::approximatelyEqual(minValue, maxValue));

    this->currentValue = minValue;
    this->target = minValue;
  }

  void reset(double sampleRate, double rampLengthSeconds) {
    const auto rampLengthSamples =
        static_cast<int>(std::floor(rampLengthSeconds * sampleRate));
    step = (b - a) / static_cast<FloatType>(rampLengthSamples);
  }

  void setTargetValue(bool goToMax) {
    this->target = goToMax ? b : a;
    step = goToMax ? std::abs(step) : -std::abs(step);
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
      : crossfadeLengthSeconds{crossfadeLengthSecondsValue} {}

  void prepare(double sampleRate,
               int channelCount,
               int expectedMaxFramesPerBlock) {
    dryBuffer.setSize(channelCount, expectedMaxFramesPerBlock);
    numSteps =
        static_cast<int>(std::floor(crossfadeLengthSeconds * sampleRate));
    dryGain.reset(sampleRate, crossfadeLengthSeconds);
    wetGain.reset(numSteps);
    reset();
  }

  void setBypass(bool bypass) noexcept {
    if (bypass == isBypassed) {
      return;
    }

    dryGain.setTargetValue(bypass);

    const auto startWetGain = bypass ? 1.f : 0.f;
    const auto endWetGain = 1.f - startWetGain;

    if (!isTransitioning()) {
      // don't change start gain if previous transition didn't complete
      wetGain.reset(numSteps);
      wetGain.setCurrentAndTargetValue(startWetGain);
    } else {
      const auto currentValue = wetGain.getCurrentValue();
      auto stepsLeftProportion = currentValue / 1.f;
      if (juce::approximatelyEqual(endWetGain, 1.f)) {
        stepsLeftProportion = 1.f - stepsLeftProportion;
      }
      const auto newNumSteps = static_cast<int>(stepsLeftProportion * numSteps);
      wetGain.reset(newNumSteps);
      wetGain.setCurrentAndTargetValue(currentValue);
    }
    wetGain.setTargetValue(endWetGain);

    isTransition = true;

    isBypassed = bypass;
  }

  [[nodiscard]] bool isTransitioning() const noexcept { return isTransition; }

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

    if (!dryGain.isSmoothing() && !wetGain.isSmoothing()) {
      isTransition = false;
    }
  }

  void reset() noexcept {
    isBypassed = false;
    isTransition = false;
    dryBuffer.clear();
  }

private:
  double crossfadeLengthSeconds;
  int numSteps{0};
  bool isBypassed = false;
  bool isTransition = false;
  juce::AudioBuffer<float> dryBuffer;
  FixedStepSmoothedValue<float> dryGain{0.f, 1.f};
  juce::SmoothedValue<float> wetGain{0.f};
};
}  // namespace tremolo
