#pragma once

#include "../colours.h"
#include "chorus_knobs_component.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class ChorusComponent : public juce::Component
{
  public:
    ChorusComponent(juce::AudioProcessorValueTreeState&);
    ~ChorusComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Label title_label;
    ChorusKnobsComponent knobs_component;

    juce::ToggleButton bypass_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypass_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusComponent)
};
