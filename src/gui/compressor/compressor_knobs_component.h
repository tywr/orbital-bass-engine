#pragma once

#include "../colours.h"
#include "../components/solid_tooltip.h"
#include "../looks/compressor_selector_look_and_feel.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct CompressorKnob
{
    juce::Slider* slider;
    juce::Label* label;
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

    CompressorSelectorLookAndFeel selector_look_and_feel;

    juce::Slider hpf_slider;
    juce::Label hpf_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        hpf_slider_attachment;

    juce::Slider threshold_slider;
    juce::Label threshold_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        threshold_slider_attachment;

    juce::Slider ratio_slider;
    juce::Label ratio_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        ratio_slider_attachment;

    juce::Slider mix_slider;
    juce::Label mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mix_slider_attachment;

    juce::Slider level_slider;
    juce::Label level_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        level_slider_attachment;

    juce::Slider attack_slider;
    juce::Label attack_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        attack_slider_attachment;

    juce::Slider release_slider;
    juce::Label release_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        release_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    // Define knobs for easy looping
    std::vector<CompressorKnob> knobs = {
        {&hpf_slider,       &hpf_label,       "compressor_hpf",       "hpf"    },
        {&threshold_slider, &threshold_label, "compressor_threshold",
         "threshold"                                                           },
        {&mix_slider,       &mix_label,       "compressor_mix",       "mix"    },
        {&level_slider,     &level_label,     "compressor_level_db",  "level"  },
        {&ratio_slider,     &ratio_label,     "compressor_ratio",     "ratio"  },
        {&attack_slider,    &attack_label,    "compressor_attack",    "attack" },
        {&release_slider,   &release_label,   "compressor_release",   "release"},
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorKnobsComponent)
};
