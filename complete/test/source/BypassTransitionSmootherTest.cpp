#include <tremolo_plugin/tremolo_plugin.h>
#include <gtest/gtest.h>

namespace tremolo {
class BypassTransitionSmootherTest : public testing::Test {
protected:
  static constexpr auto dryValue = 0;
  static constexpr auto wetValue = 10;

  void SetUp() override {
    constexpr auto sampleRate = 10;
    constexpr auto blockSize = sampleRate;
    constexpr auto channelCount = 1;
    testee.prepare(sampleRate, channelCount, blockSize);
    testee.setBypass(false);
    buffer.setSize(channelCount, blockSize);
  }

  juce::dsp::AudioBlock<float> getBlock() { return buffer; }

  BypassTransitionSmoother testee{1.0};
  juce::AudioBuffer<float> buffer;
};

TEST_F(BypassTransitionSmootherTest, OffOnTransitionIsSmooth) {
  testee.setBypass(true);
  ASSERT_TRUE(testee.isTransitioning());

  auto block = getBlock();
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

TEST_F(BypassTransitionSmootherTest, OnOffTransitionIsSmooth) {
  testee.setBypass(true);
  testee.setDryBuffer(buffer);
  testee.mixToWetBuffer(buffer);
  ASSERT_FALSE(testee.isTransitioning());

  testee.setBypass(false);
  ASSERT_TRUE(testee.isTransitioning());
  auto block = getBlock();
  block.fill(dryValue);
  testee.setDryBuffer(buffer);
  block.fill(wetValue);
  testee.mixToWetBuffer(buffer);

  EXPECT_FALSE(testee.isTransitioning());
  for (const auto i : std::views::iota(dryValue, wetValue)) {
    const auto expectedSample = i + 1;
    EXPECT_NEAR(expectedSample, buffer.getSample(0, i), 0.0001f);
  }
}

TEST_F(BypassTransitionSmootherTest, TogglingBypassMidOffOnTransitionIsSmooth) {
  testee.setBypass(true);
  ASSERT_TRUE(testee.isTransitioning());

  buffer.setSize(buffer.getNumChannels(), buffer.getNumSamples() / 2);
  auto block = getBlock();
  block.fill(dryValue);
  testee.setDryBuffer(buffer);
  block.fill(wetValue);
  testee.mixToWetBuffer(buffer);

  EXPECT_TRUE(testee.isTransitioning());
  for (const auto i : std::views::iota(dryValue, wetValue / 2)) {
    const auto expectedSample = wetValue - i - 1;
    EXPECT_NEAR(expectedSample, buffer.getSample(0, i), 0.0001f);
  }

  testee.setBypass(false);
  EXPECT_TRUE(testee.isTransitioning());

  block.fill(dryValue);
  testee.setDryBuffer(buffer);
  block.fill(wetValue);
  testee.mixToWetBuffer(buffer);
  EXPECT_FALSE(testee.isTransitioning());
  for (const auto i : std::views::iota(wetValue / 2, wetValue)) {
    const auto expectedSample = i + 1;
    EXPECT_NEAR(expectedSample, buffer.getSample(0, i - wetValue / 2), 0.0001f);
  }
}
}  // namespace tremolo
