#include <gtest/gtest.h>
#include <tremolo_plugin/tremolo_plugin.h>

#include <wolfsound/file/wolfsound_WavFileWriter.hpp>

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
}  // namespace ws
