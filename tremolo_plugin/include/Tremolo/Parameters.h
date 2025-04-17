#pragma once

namespace ws {
struct Parameters {
  using Container = std::vector<std::unique_ptr<juce::AudioProcessorParameter>>;

  explicit Parameters(Container&);

  juce::AudioParameterFloat& rate;
  juce::AudioParameterBool& bypassed;
  juce::AudioParameterChoice& waveform;

  JUCE_DECLARE_NON_COPYABLE(Parameters)
};
}  // namespace ws
