#pragma once

#include "../colours.h"
#include "../components/solid_tooltip.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct SynthVoiceKnob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};

class SynthVoiceKnobsComponent : public juce::Component
{
  public:
    SynthVoiceKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~SynthVoiceKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged;

    juce::Colour const default_type_colour = ColourCodes::grey3;

    juce::Slider raw_slider;
    juce::Label raw_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        raw_slider_attachment;

    juce::Slider square_slider;
    juce::Label square_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        square_slider_attachment;

    juce::Slider triangle_slider;
    juce::Label triangle_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        triangle_slider_attachment;

    juce::Slider octave_slider;
    juce::Label octave_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        octave_slider_attachment;

    juce::Slider master_slider;
    juce::Label master_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        master_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<SynthVoiceKnob> knobs = {
        {&raw_slider,      &raw_label,      "synth_raw_level",      "raw"     },
        {&square_slider,   &square_label,   "synth_square_level",   "square"  },
        {&triangle_slider, &triangle_label, "synth_triangle_level", "triangle"},
        {&octave_slider,   &octave_label,   "synth_octave_level",   "octave"  },
        {&master_slider,   &master_label,   "synth_master_level",   "master"  },
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoiceKnobsComponent)
};
