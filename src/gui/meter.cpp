#include "meter.h"
#include <juce_audio_basics/juce_audio_basics.h>

Meter::Meter(juce::Value& v) : measured_value(v)
{
    startTimerHz(60);

    addAndMakeVisible(slider);
    slider.setRange(-48.0f, 6.0f, 0.01f);
    slider.setSkewFactor(1.0);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setSliderStyle(juce::Slider::LinearBarVertical);

    measured_value.addListener(this);
    measured_value.setValue(-48.0f);

    smoothed_value = measured_value.getValue();
    target_value = smoothed_value;
    peak_hold_value = smoothed_value;
    peak_hold_counter = 0;
}

Meter::~Meter()
{
    measured_value.removeListener(this);
}

void Meter::resized()
{
    slider.setBounds(getLocalBounds());
}

void Meter::timerCallback()
{
    const float attack_coeff = 0.5f;   // reacts quickly to rising levels
    const float release_coeff = 0.08f; // slower decay

    if (target_value > smoothed_value)
        smoothed_value += (target_value - smoothed_value) * attack_coeff;
    else
        smoothed_value += (target_value - smoothed_value) * release_coeff;

    // --- Peak hold section ---
    const int hold_frames = 30;   // hold peak ~0.5s @ 60Hz
    const float fall_rate = 2.0f; // how fast peak falls after hold

    if (smoothed_value > peak_hold_value)
    {
        peak_hold_value = smoothed_value;
        peak_hold_counter = hold_frames;
    }
    else
    {
        if (peak_hold_counter > 0)
            --peak_hold_counter;
        else
            peak_hold_value -= fall_rate; // drop gradually
    }

    // Prevent unrealistic negative infinity values
    peak_hold_value = juce::jlimit(-48.0f, 6.0f, peak_hold_value);
    smoothed_value = juce::jlimit(-48.0f, 6.0f, smoothed_value);

    slider.setValue(smoothed_value, juce::dontSendNotification);
}

void Meter::visibilityChanged()
{
    juce::MessageManager::callAsync(
        [this]
        {
            if (isShowing())
                startTimerHz(60);
            else
                stopTimer();
        }
    );
}

void Meter::valueChanged(juce::Value& newValue)
{
    float gain = newValue.getValue();
    float dbValue = juce::Decibels::gainToDecibels(gain);
    target_value = dbValue;
}

void Meter::setSliderColour(juce::Colour c)
{
    slider.setColour(juce::Slider::trackColourId, c);
}
