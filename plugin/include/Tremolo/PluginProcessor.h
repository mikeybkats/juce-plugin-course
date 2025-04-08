#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters.h"
#include "Tremolo.h"

namespace ws {
class AudioPluginAudioProcessor : public juce::AudioProcessor {
public:
  explicit AudioPluginAudioProcessor(
      Parameters::Container parameterContainer = {});
  ~AudioPluginAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  using AudioProcessor::processBlock;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  [[nodiscard]] Parameters& getParameters() noexcept;
  juce::AudioProcessorParameter* getBypassParameter() const noexcept override;

  void readAllLfoSamples(juce::AudioBuffer<float>& bufferToFill);

  /** @brief Retrieves the sample rate the processor was initialized with in a
   * thread-safe manner */
  double getSampleRateThreadSafe() const noexcept;

private:
  Parameters parameters;
  Tremolo tremolo;
  std::atomic<double> currentSampleRate{0.};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};
}  // namespace ws
