#pragma once

#include "../colours.h"
#include "compressor_dimensions.h"
#include "compressor_meter_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

class CompressorMeterComponent : public juce::Component,
                                 public juce::Value::Listener,
                                 public juce::Timer
{
  public:
    CompressorMeterComponent(juce::Value& v);
    ~CompressorMeterComponent() override;

    void resized() override;
    void valueChanged(juce::Value& v) override;
    void paint(juce::Graphics&) override;
    void visibilityChanged() override;
    void switchColour(juce::Colour colour1, juce::Colour colour2);

  private:
    void timerCallback() override;
    juce::Value gain_reduction_value;
    juce::Slider gain_reduction_slider;

    float target_meter_value = 0.0f;
    float smoothed_meter_value = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorMeterComponent)
};
