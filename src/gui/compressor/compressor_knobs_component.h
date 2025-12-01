#pragma once

#include "../colours.h"
#include "../components/labeled_knob.h"
#include "../components/solid_tooltip.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct CompressorKnob
{
    LabeledKnob* knob;
    juce::String parameter_id;
    juce::String label_text;
};

class CompressorKnobsComponent : public juce::Component
{
  public:
    CompressorKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~CompressorKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged;

    juce::Colour const default_type_colour = ColourCodes::grey3;

    LabeledKnob hpf_knob;
    LabeledKnob threshold_knob;
    LabeledKnob ratio_knob;
    LabeledKnob mix_knob;
    LabeledKnob level_knob;
    LabeledKnob attack_knob;
    LabeledKnob release_knob;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    // Define knobs for easy looping
    std::vector<CompressorKnob> knobs = {
        {&hpf_knob,       "compressor_hpf",       "hpf"    },
        {&threshold_knob, "compressor_threshold", "threshold"},
        {&mix_knob,       "compressor_mix",       "mix"    },
        {&level_knob,     "compressor_level_db",  "level"  },
        {&ratio_knob,     "compressor_ratio",     "ratio"  },
        {&attack_knob,    "compressor_attack",    "attack" },
        {&release_knob,   "compressor_release",   "release"},
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorKnobsComponent)
};
