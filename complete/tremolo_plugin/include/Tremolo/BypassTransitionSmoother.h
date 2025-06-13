#pragma once

namespace tremolo {
template <typename FloatType>
class FixedStepRangedSmoothedValue
    : public juce::SmoothedValueBase<FixedStepRangedSmoothedValue<FloatType>> {
public:
  explicit FixedStepRangedSmoothedValue(
      juce::Range<FloatType> rangeToUse) noexcept
      : range{rangeToUse} {
    jassert(range.getStart() < range.getEnd());
    jassert(!juce::approximatelyEqual(range.getStart(), range.getEnd()));

    this->setCurrentAndTargetValue(range.getStart());
  }

  void reset(double sampleRate, double rampLengthSeconds) noexcept {
    const auto rampLengthSamples =
        static_cast<int>(std::floor(rampLengthSeconds * sampleRate));
    step = range.getLength() / static_cast<FloatType>(rampLengthSamples);
  }

  void setTargetValueToExtreme(bool goToMax) noexcept {
    this->target = goToMax ? range.getEnd() : range.getStart();
    step =
        (this->currentValue < this->target) ? std::abs(step) : -std::abs(step);
    this->countdown =
        static_cast<int>(std::abs((this->currentValue - this->target) / step));
  }

  void setCurrentAndTargetValueToExtreme(bool setToMax) noexcept {
    this->setCurrentAndTargetValue(setToMax ? range.getEnd()
                                            : range.getStart());
  }

  FloatType getNextValue() noexcept {
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

  [[nodiscard]] juce::Range<FloatType> getRange() const noexcept {
    return range;
  }

private:
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
    dryGain.setCurrentAndTargetValue(dryGain.getRange().getStart());
    wetGain.setCurrentAndTargetValue(wetGain.getRange().getEnd());
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

    dryGain.setTargetValueToExtreme(bypass);
    wetGain.setTargetValueToExtreme(!bypass);

    isBypassed = bypass;
  }

  void setBypassForced(bool bypass) noexcept {
    isBypassed = bypass;

    dryGain.setCurrentAndTargetValueToExtreme(isBypassed);
    wetGain.setCurrentAndTargetValueToExtreme(!isBypassed);
  }

  [[nodiscard]] bool isTransitioning() const noexcept {
    return dryGain.isSmoothing() || wetGain.isSmoothing();
  }

  void setDryBuffer(const juce::AudioBuffer<float>& buffer) noexcept {
    jassert(buffer.getNumSamples() <= dryBuffer.getNumSamples());
    jassert(buffer.getNumChannels() <= dryBuffer.getNumChannels());

    for (const auto channel : std::views::iota(0, buffer.getNumChannels())) {
      dryBuffer.copyFrom(channel, 0, buffer, channel, 0,
                         buffer.getNumSamples());
    }
    dryGain.applyGain(dryBuffer, buffer.getNumSamples());
  }

  void mixToWetBuffer(juce::AudioBuffer<float>& buffer) noexcept {
    if (!isTransitioning() && !isBypassed) {
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
    isBypassed = false;
    dryBuffer.clear();
  }

private:
  double crossfadeLengthSeconds;
  bool isBypassed = false;
  juce::AudioBuffer<float> dryBuffer;
  FixedStepRangedSmoothedValue<float> dryGain{{0.f, 1.f}};
  FixedStepRangedSmoothedValue<float> wetGain{{0.f, 1.f}};
};
}  // namespace tremolo
