namespace ws {
namespace {
struct ParameterIdentifiers {
  juce::Identifier pluginName;

  /** @brief Version of the parameters JSON */
  juce::Identifier version;

  /** @brief Modulation rate parameter */
  juce::Identifier rate;

  /** @brief Bypass parameter */
  juce::Identifier bypass;

  juce::Identifier waveform;
};

const ParameterIdentifiers& getIdentifiers() {
  static const ParameterIdentifiers ids{
      .pluginName = "pluginName",
      .version = "version",
      .rate = "modulationRateHz",
      .bypass = "bypassed",
      .waveform = "modulationWaveform",
  };
  return ids;
}

constexpr auto pluginName = PLUGIN_NAME;
constexpr auto currentVersion = "1.0.0";
}  // namespace

void JsonSerializer::serialize(const Parameters& parameters,
                               juce::OutputStream& output) {
  juce::DynamicObject json{};
  const auto& ids = getIdentifiers();

  json.setProperty(ids.pluginName, pluginName);
  json.setProperty(ids.version, currentVersion);
  json.setProperty(ids.rate, static_cast<double>(parameters.rate.get()));
  json.setProperty(ids.bypass, parameters.bypassed.get());
  json.setProperty(ids.waveform, parameters.waveform.getCurrentChoiceName());

  json.writeAsJSON(output, juce::JSON::FormatOptions{}
                               .withSpacing(juce::JSON::Spacing::multiLine)
                               .withMaxDecimalPlaces(2));
}

void JsonSerializer::deserialize(juce::InputStream& input,
                                 Parameters& parameters) {
  juce::var parsedResult;
  const auto parsingResult =
      juce::JSON::parse(input.readEntireStreamAsString(), parsedResult);

  if (parsingResult.failed()) {
    DBG(parsingResult.getErrorMessage());
    return;
  }

  if (!parsedResult.isObject()) {
    DBG("not a dynamic object");
    return;
  }

  const auto& ids = getIdentifiers();
  if (!parsedResult.hasProperty(ids.pluginName) ||
      parsedResult[ids.pluginName] != pluginName) {
    DBG("invalid plugin name");
    return;
  }

  if (!parsedResult.hasProperty(ids.version) ||
      parsedResult[ids.version] != currentVersion) {
    DBG("this plugin version only supports version " << currentVersion
                                                     << " of parameters");
    return;
  }

  parameters.rate = parsedResult.getProperty(
      ids.rate, static_cast<double>(parameters.rate.get()));
  parameters.bypassed =
      parsedResult.getProperty(ids.bypass, parameters.bypassed.get());

  if (parsedResult.hasProperty(ids.waveform)) {
    const auto waveformName = parsedResult[ids.waveform];
    if (waveformName.isString()) {
      const auto index =
          parameters.waveform.choices.indexOf(waveformName.toString());
      if (0 <= index) {
        parameters.waveform = index;
      }
    }
  }
}
}  // namespace ws
