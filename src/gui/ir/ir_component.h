#pragma once

#include "../../assets/impulse_response_binary.h"
#include "../colours.h"
#include "../components/solid_tooltip.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

struct IRKnob
{
    juce::Slider* slider;
    juce::Label* label;
    juce::String parameter_id;
    juce::String label_text;
};

class IRComponent : public juce::Component
{
  public:
    IRComponent(juce::AudioProcessorValueTreeState&);
    ~IRComponent() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void refreshStatus();
    void switchColour();
    void setupSliderTooltipHandling(juce::Slider* slider, juce::Label* label);

  private:
    juce::AudioProcessorValueTreeState& parameters;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged = false;

    juce::Colour current_colour = ColourCodes::white0;

    juce::ToggleButton bypassButton;
    juce::Label bypassLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment;

    juce::Slider ir_mix_slider;
    juce::Label ir_mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        ir_mix_sliderAttachment;

    juce::Slider gain_slider;
    juce::Label gain_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gain_sliderAttachment;

    juce::Label type_label;
    juce::Slider type_slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        type_slider_attachment;

    juce::DrawableText type_display;
    std::unique_ptr<juce::ParameterAttachment> type_display_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<IRKnob> knobs = {
        {&ir_mix_slider, &ir_mix_label, "ir_mix",   "mix" },
        {&gain_slider,   &gain_label,   "ir_level", "gain"},
        {&type_slider,   &type_label,   "ir_type",  "type"},
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRComponent);
};
