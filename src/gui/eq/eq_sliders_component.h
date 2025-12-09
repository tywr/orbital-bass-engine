#pragma once

#include "../colours.h"
#include "../components/labeled_knob.h"
#include "../components/solid_tooltip.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct EqSlider
{
    LabeledKnob* knob;
    juce::String parameter_id;
    juce::String label_text;
};

class EqSlidersComponent : public juce::Component
{
  public:
    EqSlidersComponent(juce::AudioProcessorValueTreeState&);
    ~EqSlidersComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Colour const default_type_colour = ColourCodes::grey3;

    LabeledKnob low_shelf_gain_knob;
    LabeledKnob low_shelf_freq_knob;
    LabeledKnob low_mid_freq_knob;
    LabeledKnob low_mid_q_knob;
    LabeledKnob low_mid_gain_knob;
    LabeledKnob high_mid_freq_knob;
    LabeledKnob high_mid_q_knob;
    LabeledKnob high_mid_gain_knob;
    LabeledKnob high_shelf_gain_knob;
    LabeledKnob high_shelf_freq_knob;
    LabeledKnob lpf_knob;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged;

    std::vector<EqSlider> sliders = {
        {&low_shelf_gain_knob,  "eq_low_shelf_gain",  "low"  },
        {&low_shelf_freq_knob,  "eq_low_shelf_freq",  "freq" },
        {&low_mid_freq_knob,    "eq_low_mid_freq",    "freq" },
        {&low_mid_q_knob,       "eq_low_mid_q",       "q"    },
        {&low_mid_gain_knob,    "eq_low_mid_gain",    "gain" },
        {&high_mid_freq_knob,   "eq_high_mid_freq",   "freq" },
        {&high_mid_q_knob,      "eq_high_mid_q",      "q"    },
        {&high_mid_gain_knob,   "eq_high_mid_gain",   "gain" },
        {&high_shelf_gain_knob, "eq_high_shelf_gain", "highs"},
        {&high_shelf_freq_knob, "eq_high_shelf_freq", "freq" },
        {&lpf_knob,             "eq_lpf",             "lpf"  },
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqSlidersComponent)
};
