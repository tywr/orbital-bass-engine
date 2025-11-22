#pragma once

#include "../colours.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct EqSlider
{
    juce::Slider* slider;
    juce::Label* label;
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

    juce::Slider low_shelf_gain_slider;
    juce::Label low_shelf_gain_label;

    juce::Slider low_mid_freq_slider;
    juce::Label low_mid_freq_label;
    juce::Slider low_mid_q_slider;
    juce::Label low_mid_q_label;
    juce::Slider low_mid_gain_slider;
    juce::Label low_mid_gain_label;

    juce::Slider high_mid_freq_slider;
    juce::Label high_mid_freq_label;
    juce::Slider high_mid_q_slider;
    juce::Label high_mid_q_label;
    juce::Slider high_mid_gain_slider;
    juce::Label high_mid_gain_label;

    juce::Slider high_shelf_gain_slider;
    juce::Label high_shelf_gain_label;

    juce::Slider lpf_slider;
    juce::Label lpf_label;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<EqSlider> sliders = {
        {&low_shelf_gain_slider,  &low_shelf_gain_label,  "eq_low_shelf_gain",
         "low"                                                                        },
        {&low_mid_freq_slider,    &low_mid_freq_label,    "eq_low_mid_freq",    "freq"},
        {&low_mid_q_slider,       &low_mid_q_label,       "eq_low_mid_q",       "q"   },
        {&low_mid_gain_slider,    &low_mid_gain_label,    "eq_low_mid_gain",    "gain"},
        {&high_mid_freq_slider,   &high_mid_freq_label,   "eq_high_mid_freq",
         "freq"                                                                       },
        {&high_mid_q_slider,      &high_mid_q_label,      "eq_high_mid_q",      "q"   },
        {&high_mid_gain_slider,   &high_mid_gain_label,   "eq_high_mid_gain",
         "gain"                                                                       },
        {&high_shelf_gain_slider, &high_shelf_gain_label, "eq_high_shelf_gain",
         "highs"                                                                      },
        {&lpf_slider,             &lpf_label,             "eq_lpf",             "lpf" },
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqSlidersComponent)
};
