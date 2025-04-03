#pragma once
#include <vector>
#include <memory>
#include <juce_audio_processors/juce_audio_processors.h>

namespace ws {
struct Parameters {
  using Container = std::vector<std::unique_ptr<juce::AudioProcessorParameter>>;
  explicit Parameters(Container&);

  juce::AudioParameterFloat& rate;
  juce::AudioParameterBool& bypassed;

  JUCE_DECLARE_NON_COPYABLE(Parameters)
};
}  // namespace ws
