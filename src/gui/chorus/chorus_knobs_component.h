#pragma once

#include "../colours.h"
#include "../components/labeled_knob.h"
#include "../components/solid_tooltip.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct ChorusKnob
{
    LabeledKnob* knob;
    juce::String parameter_id;
    juce::String label_text;
};

class ChorusKnobsComponent : public juce::Component
{
  public:
    ChorusKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~ChorusKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged;

    juce::Colour const default_type_colour = ColourCodes::grey3;

    LabeledKnob rate_knob;
    LabeledKnob depth_knob;
    LabeledKnob mix_knob;
    LabeledKnob crossover_knob;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<ChorusKnob> knobs = {
        {&rate_knob,      "chorus_rate",      "rate"  },
        {&depth_knob,     "chorus_depth",     "depth" },
        {&crossover_knob, "chorus_crossover", "freq"  },
        {&mix_knob,       "chorus_mix",       "mix"   }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusKnobsComponent)
};
