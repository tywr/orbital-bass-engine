#pragma once

#include "colours.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class IRLoader : public juce::Component
{
  public:
    IRLoader(juce::AudioProcessorValueTreeState&);
    ~IRLoader() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void refreshStatus();
    void switchColour();

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRLoader)
};
