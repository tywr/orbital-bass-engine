#pragma once

#include "amp/amp_component.h"
#include "compressor/compressor_component.h"
#include "post_rack.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Panels : public juce::Component
{
  public:
    Panels(juce::AudioProcessorValueTreeState&, juce::Value&);
    ~Panels() override;

    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    CompressorComponent compressor_component;
    AmpComponent amp_component;
    PostRackComponent post_rack_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Panels)
};
