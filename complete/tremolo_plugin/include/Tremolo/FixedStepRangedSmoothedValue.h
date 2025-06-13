#pragma once

namespace tremolo {
/** @brief A SmoothedValue-like class that moves with a fixed step
 * between 2 extreme values.
 *
 * This is used to provide a fixed smoothing curve instead of
 * a precise smoothing length. In other words, the smoothed value
 * always moves along a curve you can specify with the range
 * and the ramp duration.
 * The specified ramp duration is
 * guaranteed only if the value is smoothed from one extreme
 * to the other.
 *
 * If you set the value outside the specified range using
 * setCurrentAndTargetValue(), the smoothing will
 * progress along the specified curve.
 *
 * @see BypassTransitionSmoother
 */
template <typename FloatType>
class FixedStepRangedSmoothedValue
    : public juce::SmoothedValueBase<FixedStepRangedSmoothedValue<FloatType>> {
public:
  explicit FixedStepRangedSmoothedValue(
      juce::Range<FloatType> rangeToUse) noexcept
      : range{rangeToUse} {
    jassert(range.getStart() < range.getEnd());
    jassert(!juce::approximatelyEqual(range.getStart(), range.getEnd()));

    setCurrentAndTargetValueToExtreme(false);
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
}  // namespace tremolo
