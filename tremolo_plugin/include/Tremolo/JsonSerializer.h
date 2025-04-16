#pragma once

namespace ws {
class JsonSerializer {
public:
  static void serialize(const Parameters&, juce::OutputStream&);
  static void deserialize(juce::InputStream&, Parameters&);
};
}  // namespace ws
