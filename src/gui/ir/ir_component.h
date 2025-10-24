#pragma once

#include "../../assets/impulse_response_binary.h"
#include "../colours.h"
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

  private:
    juce::AudioProcessorValueTreeState& parameters;
    void chooseFile();
    juce::String choosenFilePath;
    juce::Colour iRColour = ColourCodes::white0;

    juce::ToggleButton bypassButton;
    juce::Label bypassLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment;
    juce::TextButton loadButton;
    juce::Label statusLabel;
    std::unique_ptr<juce::FileChooser> chooser;

    juce::Slider irMixSlider;
    juce::Label irMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        irMixSliderAttachment;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gainSliderAttachment;

    juce::Slider type_slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        type_slider_attachment;

    IRType modern_410_type = {
        &modern_410_button, "modern_410", ImpulseResponseBinary::modern_410_wav,
        ImpulseResponseBinary::modern_410_wavSize
    };
    Modern410ToggleButton modern_410_button =
        Modern410ToggleButton(modern_410_type);

    std::vector<IRType> types = {modern_410_type};
    IRType selected_type = types[0];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRComponent);
};
