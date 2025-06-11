#include <tremolo_plugin/tremolo_plugin.h>
#include <gtest/gtest.h>

namespace tremolo {
TEST(BypassTransitionSmoother, TransitionIsSmooth) {
  BypassTransitionSmoother testee{1.0};
  constexpr auto sampleRate = 10;
  constexpr auto blockSize = sampleRate;
  constexpr auto channelCount = 1;
  testee.prepare(sampleRate, channelCount, blockSize);
  testee.setBypass(false);
  juce::AudioBuffer<float> buffer;
  buffer.setSize(channelCount, blockSize);
  juce::dsp::AudioBlock<float> block{buffer};
  testee.setBypass(true);
  ASSERT_TRUE(testee.isTransitioning());
  constexpr auto dryValue = 0;
  constexpr auto wetValue = 10;
  block.fill(dryValue);
  testee.setDryBuffer(buffer);
  block.fill(wetValue);
  testee.mixToWetBuffer(buffer);
  EXPECT_FALSE(testee.isTransitioning());
  for (const auto i : std::views::iota(dryValue, wetValue)) {
    const auto expectedSample = wetValue - i - 1;
    EXPECT_NEAR(expectedSample, buffer.getSample(0, i), 0.0001f);
  }
}
}  // namespace tremolo
