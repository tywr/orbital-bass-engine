#pragma once

#include "amp/amp_component.h"
#include "chorus/chorus_component.h"
#include "compressor/compressor_component.h"
#include "ir/ir_component.h"
#include "post_rack.h"
#include "synth/synth_component.h"
#include "tabs.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Tabs : public juce::TabbedComponent
{
  public:
    Tabs(juce::AudioProcessorValueTreeState&, juce::Value&);
    ~Tabs() override;
    void paint(juce::Graphics&) override;

  private:
    CompressorComponent compressor_component;
    AmpComponent amp_component;
    ChorusComponent chorus_component;
    IRComponent ir_component;
    PostRackComponent post_rack_component;
    SynthComponent synth_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tabs)
};
