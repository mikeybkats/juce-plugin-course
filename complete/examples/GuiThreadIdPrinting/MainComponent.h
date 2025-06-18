#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <bit>

class GuiThreadIdPrinting : public juce::Component {
public:
  GuiThreadIdPrinting() {
    DBG("Main component constructor thread ID: " << getCurrentThreadId());
  }

private:
  [[nodiscard]] static juce::String getCurrentThreadId() {
    return juce::String::toHexString(
        std::bit_cast<juce::int64>(juce::Thread::getCurrentThreadId()));
  }
};

using MainComponent = GuiThreadIdPrinting;
