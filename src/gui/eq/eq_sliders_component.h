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

    juce::Slider b80_slider;
    juce::Label b80_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        b80_slider_attachment;

    juce::Slider b250_slider;
    juce::Label b250_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        b250_slider_attachment;

    juce::Slider b500_slider;
    juce::Label b500_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        b500_slider_attachment;

    juce::Slider b1500_slider;
    juce::Label b1500_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        b1500_slider_attachment;

    juce::Slider b3000_slider;
    juce::Label b3000_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        b3000_slider_attachment;

    juce::Slider b5000_slider;
    juce::Label b5000_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        b5000_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<EqSlider> sliders = {
        {&b80_slider,   &b80_label,   "eq_b80",   "80"  },
        {&b250_slider,  &b250_label,  "eq_b250",  "250" },
        {&b500_slider,  &b500_label,  "eq_b500",  "500" },
        {&b1500_slider, &b1500_label, "eq_b1500", "1.5k"},
        {&b3000_slider, &b3000_label, "eq_b3000", "3k"  },
        {&b5000_slider, &b5000_label, "eq_b5000", "5k"  },
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqSlidersComponent)
};
