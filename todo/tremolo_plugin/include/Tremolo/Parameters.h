#pragma once

namespace tremolo {
struct Parameters {
  using Container = std::vector<std::unique_ptr<juce::AudioProcessorParameter>>;

  explicit Parameters(Container&);
};
}  // namespace tremolo
