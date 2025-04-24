#include <gtest/gtest.h>
#include <tremolo_plugin/tremolo_plugin.h>

#include <wolfsound/file/wolfsound_WavFileWriter.hpp>
#include <wolfsound/dsp/wolfsound_testSignals.hpp>

namespace ws {
/** This test extracts the LFO used by the Tremolo effect and saves it to a WAV
 * file.
 *
 * The LFO rate is not changed; thus, the one set in Tremolo's constructor will
 * be used.
 */
TEST(Tremolo, ExtractLfo) {
  Tremolo testee;
  constexpr auto sampleRate = 48000.0;
  testee.prepare(sampleRate, static_cast<int>(sampleRate));

  juce::AudioBuffer<float> buffer;
  buffer.setSize(1, static_cast<int>(sampleRate));

  juce::dsp::AudioBlock<float> block{buffer};
  block.fill(1.f);
  testee.process(buffer);
  block.subtract(1.f);

  wolfsound::WavFileWriter::writeToFile(
      juce::File::getSpecialLocation(
          juce::File::SpecialLocationType::currentExecutableFile)
          .getParentDirectory()
          .getChildFile("lfo.wav")
          .getFullPathName()
          .toStdString(),
      juce::Span<const float>{block.getChannelPointer(0u),
                              block.getNumSamples()},
      wolfsound::Frequency{sampleRate});
}

TEST(Tremolo, SamplewiseAndChannelwiseProcessingYieldIdenticalResults) {
  using namespace wolfsound::literals;
  using namespace std::chrono_literals;

  const auto sampleRate = 48000_Hz;
  const auto testSignal = wolfsound::generateWhiteNoise(sampleRate, 1s, 0);

  // create 2 identical input signal buffers
  juce::AudioBuffer<float> samplewiseBuffer{
      1, static_cast<int>(testSignal.size())};
  std::ranges::copy(testSignal, samplewiseBuffer.getWritePointer(0));
  auto channelwiseBuffer = samplewiseBuffer;

  // create 2 identical Tremolo instances with default parameters
  Tremolo samplewiseTremolo;
  Tremolo channelwiseTremolo;

  samplewiseTremolo.prepare(sampleRate.value(),
                            static_cast<int>(testSignal.size()));
  channelwiseTremolo.prepare(sampleRate.value(),
                             static_cast<int>(testSignal.size()));

  // process the first using sample-wise processing
  samplewiseTremolo.process(samplewiseBuffer);

  // process the second using block-wise processing
  channelwiseTremolo.processChannelwise(channelwiseBuffer);

  for (const auto i : std::views::iota(0, samplewiseBuffer.getNumSamples())) {
    EXPECT_NEAR(samplewiseBuffer.getSample(0, i),
                channelwiseBuffer.getSample(0, i), 1e-5f);
  }
}
}  // namespace ws
