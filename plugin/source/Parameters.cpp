#include "Tremolo/Parameters.h"

namespace ws {
namespace {
juce::AudioParameterFloat& createModulationRateParameter(
    Parameters::Container& c) {
  auto parameter = std::make_unique<juce::AudioParameterFloat>(
      "modulation.rate", "Modulation rate",
      juce::NormalisableRange<float>{0.1f, 20.f}, 5.f,
      juce::AudioParameterFloatAttributes{}.withLabel("Hz"));
  auto& parameterReference = *parameter;
  c.push_back(std::move(parameter));
  return parameterReference;
}

juce::AudioParameterBool& createBypassedParameter(Parameters::Container& c) {
  auto parameter = std::make_unique<juce::AudioParameterBool>(
      "bypassed", "Bypass", false,
      juce::AudioParameterBoolAttributes{}.withStringFromValueFunction(
          [](auto isBypassed, auto) { return isBypassed ? "Off" : "On"; }));
  auto& parameterReference = *parameter;
  c.push_back(std::move(parameter));
  return parameterReference;
}
}  // namespace

Parameters::Parameters(Container& parameterContainer)
    : rate{createModulationRateParameter(parameterContainer)},
      bypassed{createBypassedParameter(parameterContainer)} {}
}  // namespace ws
