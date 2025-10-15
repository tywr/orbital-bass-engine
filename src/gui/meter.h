#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Meter : public juce::Component,
              public juce::Value::Listener,
              public juce::Timer
{
  public:
    Meter(juce::Value& v);
    ~Meter() override;

    void resized() override;
    void valueChanged(juce::Value& v) override;
    void setSliderColour(juce::Colour c);
    void visibilityChanged() override;

  private:
    void timerCallback() override;
    juce::Slider slider;
    juce::Value& measured_value;
    float smoothed_value;
    float target_value;
    float peak_hold_value;
    int peak_hold_counter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Meter)
};
