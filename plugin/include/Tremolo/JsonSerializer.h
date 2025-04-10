#pragma once

#include <juce_core/juce_core.h>
#include "Parameters.h"

namespace ws {
class JsonSerializer {
public:
  void serialize(const Parameters&, juce::OutputStream&);
  void deserialize(juce::InputStream& input, Parameters& parameters);
};
}  // namespace ws
