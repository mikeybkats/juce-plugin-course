#pragma once

#include "Parameters.h"
#include "Tremolo.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace ws {
class PluginProcessor : public juce::AudioProcessor {
public:
  explicit PluginProcessor(Parameters::Container parameterContainer = {});

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  void releaseResources() override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

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

  /** @brief Retrieves the most recent sample rate the processor was given
   * in a thread-safe manner */
  double getSampleRateThreadSafe() const noexcept;

private:
  Parameters parameters;
  Tremolo tremolo;
  std::atomic<double> currentSampleRate{0.};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
}  // namespace ws
