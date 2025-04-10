#include <Tremolo/JsonSerializer.h>
#include <Tremolo/Parameters.h>
#include <gtest/gtest.h>
#include <juce_core/juce_core.h>

namespace ws {
TEST(JsonSerializer, SerializeToFile) {
  Parameters::Container container;
  Parameters parameters{container};

  parameters.rate = 10.f;
  parameters.bypassed = true;
  parameters.waveform = 1;

  const juce::String EXPECTED_OUTPUT =
      u8R"({
  "pluginName": "Tremolo",
  "version": "1.0.0",
  "modulationRateHz": 10.0,
  "bypassed": true,
  "modulationWaveform": "Triangle"
})";
  juce::MemoryBlock block;
  juce::MemoryOutputStream outputStream{block, false};

  JsonSerializer{}.serialize(parameters, outputStream);
  outputStream.flush();

  const auto result = outputStream.toUTF8().removeCharacters("\r");

  EXPECT_EQ(EXPECTED_OUTPUT, result);
}

TEST(JsonSerializer, DeserializeFromString) {
  const juce::String SAVED_PARAMETERS =
      u8R"({
  "pluginName": "Tremolo",
  "version": "1.0.0",
  "modulationRateHz": 10.0,
  "bypassed": true,
  "modulationWaveform": "Triangle"
})";

  juce::MemoryInputStream inputStream{
      SAVED_PARAMETERS.getCharPointer(),
      static_cast<size_t>(SAVED_PARAMETERS.length()), false};

  Parameters::Container container;
  Parameters parameters{container};

  JsonSerializer{}.deserialize(inputStream, parameters);

  EXPECT_FLOAT_EQ(parameters.rate, 10.f);
  EXPECT_TRUE(parameters.bypassed);
  EXPECT_EQ(juce::String{"Triangle"},
            parameters.waveform.getCurrentChoiceName());
}
}  // namespace ws
