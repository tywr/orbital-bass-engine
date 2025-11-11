#pragma once

#include "../colours.h"
#include "eq_sliders_component.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class EqComponent : public juce::Component
{
  public:
    EqComponent(juce::AudioProcessorValueTreeState&);
    ~EqComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;

  private:
    juce::AudioProcessorValueTreeState& parameters;

    juce::Label title_label;
    EqSlidersComponent sliders_component;

    juce::ToggleButton bypass_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypass_attachment;

    juce::Slider lpf_slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lpf_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqComponent)
};
