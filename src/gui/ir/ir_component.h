#pragma once

#include "../../assets/impulse_response_binary.h"
#include "../colours.h"
#include "../components/solid_tooltip.h"
#include "ir_type.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class IRComponent : public juce::Component
{
  public:
    IRComponent(juce::AudioProcessorValueTreeState&);
    ~IRComponent() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void refreshStatus();
    void switchColour();
    void switchIR(IRType);
    void initType();
    void setupSliderTooltipHandling(
        juce::Slider* slider, juce::Label* label
    );

  private:
    juce::AudioProcessorValueTreeState& parameters;

    SolidTooltip drag_tooltip;
    bool slider_being_dragged = false;

    juce::Colour current_colour = ColourCodes::white0;

    juce::ToggleButton bypassButton;
    juce::Label bypassLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment;
    juce::TextButton loadButton;
    juce::Label statusLabel;
    std::unique_ptr<juce::FileChooser> chooser;

    juce::Slider ir_mix_slider;
    juce::Label ir_mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        ir_mix_sliderAttachment;

    juce::Slider gain_slider;
    juce::Label gain_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gain_sliderAttachment;

    juce::Slider type_slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        type_slider_attachment;

    IRType modern_410_type = {&modern_410_button, "modern_410"};
    Modern410ToggleButton modern_410_button =
        Modern410ToggleButton(modern_410_type);

    IRType classic_810_type = {
        &classic_810_button,
        "classic_810",
    };
    Classic810ToggleButton classic_810_button =
        Classic810ToggleButton(classic_810_type);

    IRType crunchy_212_type = {
        &crunchy_212_button,
        "crunchy_212",
    };
    Crunchy212ToggleButton crunchy_212_button =
        Crunchy212ToggleButton(crunchy_212_type);

    IRType vintage_b15_type = {
        &vintage_b15_button,
        "vintage_b15",
    };
    VintageB15ToggleButton vintage_b15_button =
        VintageB15ToggleButton(vintage_b15_type);

    std::vector<IRType> types = {
        modern_410_type, crunchy_212_type, vintage_b15_type, classic_810_type
    };
    IRType selected_type = types[0];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRComponent);
};
