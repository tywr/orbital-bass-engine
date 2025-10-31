#pragma once

#include "../colours.h"
#include "../components/solid_tooltip.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct FuzzKnob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};

class FuzzKnobsComponent : public juce::Component
{
  public:
    FuzzKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~FuzzKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged;

    juce::Colour const default_type_colour = ColourCodes::grey3;

    juce::Slider tone_slider;
    juce::Label tone_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        tone_slider_attachment;

    juce::Slider sustain_slider;
    juce::Label sustain_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        sustain_slider_attachment;

    juce::Slider mix_slider;
    juce::Label mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mix_slider_attachment;

    juce::Slider level_slider;
    juce::Label level_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        level_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<FuzzKnob> knobs = {
        {&tone_slider,    &tone_label,    "fuzz_tone",    "tone"   },
        {&sustain_slider, &sustain_label, "fuzz_sustain", "sustain"},
        {&mix_slider,     &mix_label,     "fuzz_mix",     "mix"    },
        {&level_slider,   &level_label,   "fuzz_level",   "level"  }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FuzzKnobsComponent)
};
