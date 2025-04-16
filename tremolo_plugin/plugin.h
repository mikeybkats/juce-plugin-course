/*
==============================================================================

BEGIN_JUCE_MODULE_DECLARATION

   ID:            plugin
   vendor:        WolfSound
   version:       1.0.0
   name:          Tremolo Plugin
   description:   The cores of the tremolo plugin as a JUCE module
   dependencies:  juce_audio_utils, juce_dsp

   website:       https://thewolfsound.com
   license:       MIT

   END_JUCE_MODULE_DECLARATION

==============================================================================
*/

#pragma once

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <type_traits>
#include <juce_core/juce_core.h>
#include <vector>
#include <memory>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>
#include <juce_dsp/juce_dsp.h>
#include <functional>
#include <ranges>
#include <algorithm>
#include <array>
#include <cmath>

#include "include/Tremolo/Parameters.h"
#include "include/Tremolo/CustomLookAndFeel.h"
#include "include/Tremolo/GradientComponent.h"
#include "include/Tremolo/JsonSerializer.h"
#include "include/Tremolo/LfoVisualizer.h"
#include "include/Tremolo/SampleFifo.h"
#include "include/Tremolo/Tremolo.h"
#include "include/Tremolo/PluginProcessor.h"
#include "include/Tremolo/PluginEditor.h"
