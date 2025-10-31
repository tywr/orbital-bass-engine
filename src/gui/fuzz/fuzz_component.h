#pragma once

#include "../colours.h"
#include "fuzz_knobs_component.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class FuzzComponent : public juce::Component
{
  public:
    FuzzComponent(juce::AudioProcessorValueTreeState&);
    ~FuzzComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Label title_label;
    FuzzKnobsComponent knobs_component;

    juce::ToggleButton bypass_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypass_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FuzzComponent)
};
