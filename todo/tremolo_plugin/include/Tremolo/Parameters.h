#pragma once

namespace ws {
struct Parameters {
  using Container = std::vector<std::unique_ptr<juce::AudioProcessorParameter>>;

  explicit Parameters(Container&);
};
}  // namespace ws
