#pragma once

#include "../colours.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct ChorusKnob
{
    juce::Slider* slider;
    juce::Label* label;
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

    juce::Colour const default_type_colour = ColourCodes::grey3;

    juce::Slider rate_slider;
    juce::Label rate_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        rate_slider_attachment;

    juce::Slider depth_slider;
    juce::Label depth_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        depth_slider_attachment;

    juce::Slider mix_slider;
    juce::Label mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mix_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<ChorusKnob> knobs = {
        {&rate_slider,  &rate_label,  "chorus_rate",  "rate" },
        {&depth_slider, &depth_label, "chorus_depth", "depth"},
        {&mix_slider,   &mix_label,   "chorus_mix",   "mix"  }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusKnobsComponent)
};
