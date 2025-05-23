
namespace ws {
namespace {
juce::AudioParameterFloat& createModulationRateParameter(
    Parameters::Container& c) {
  constexpr auto versionHint = 1;
  auto parameter = std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"modulation.rate", versionHint}, "Modulation rate",
      juce::NormalisableRange<float>{0.1f, 20.f, 0.01f, 0.4f}, 5.f,
      juce::AudioParameterFloatAttributes{}.withLabel("Hz"));
  auto& parameterReference = *parameter;
  c.push_back(std::move(parameter));
  return parameterReference;
}

juce::AudioParameterBool& createBypassedParameter(Parameters::Container& c) {
  constexpr auto versionHint = 1;
  auto parameter = std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{"bypassed", versionHint}, "Bypass", false);
  auto& parameterReference = *parameter;
  c.push_back(std::move(parameter));
  return parameterReference;
}

juce::AudioParameterChoice& createWaveformParameter(Parameters::Container& c) {
  constexpr auto versionHint = 1;
  auto parameter = std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{"modulation.waveform", versionHint},
      "Modulation waveform", juce::StringArray{"Sine", "Triangle"}, 0);
  auto& parameterReference = *parameter;
  c.push_back(std::move(parameter));
  return parameterReference;
}
}  // namespace

Parameters::Parameters(Container& parameterContainer)
    : rate{createModulationRateParameter(parameterContainer)},
      bypassed{createBypassedParameter(parameterContainer)},
      waveform{createWaveformParameter(parameterContainer)} {}
}  // namespace ws
