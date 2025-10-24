#pragma once

#include "../colours.h"
#include "compressor_knobs_component.h"
#include "compressor_meter_component.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class CompressorComponent : public juce::Component
{
  public:
    CompressorComponent(juce::AudioProcessorValueTreeState&, juce::Value&);
    ~CompressorComponent() override;

    void resized() override;
    void paint(juce::Graphics&) override;
    void visibilityChanged() override
    {
        meter_component.setVisible(isVisible());
        meter_component.visibilityChanged();
    }

  private:
    juce::AudioProcessorValueTreeState& parameters;
    juce::Value& gain_reduction_decibels;
    juce::Rectangle<float> bounds;

    juce::Image gravity_cache;

    void paintStyling(juce::Graphics&, juce::Rectangle<float>);

    // Sub-components
    juce::Label title_label;
    CompressorKnobsComponent knobs_component;
    CompressorMeterComponent meter_component;

    // Bypass button
    juce::ToggleButton bypass_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypass_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};
