#include "compressor_meter_component.h"
#include "../looks/compressor_look_and_feel.h"
#include "compressor_dimensions.h"

CompressorMeterComponent::CompressorMeterComponent(juce::Value& v)
    : gain_reduction_value(v)
{
    setLookAndFeel(new CompressorLookAndFeel());
    startTimerHz(refresh_rate);
    addAndMakeVisible(gain_reduction_slider);

    gain_reduction_slider.setRange(0, 20.0f, 0.5f);
    gain_reduction_slider.setSkewFactor(1.0);
    gain_reduction_slider.setValue(0.0);
    gain_reduction_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    gain_reduction_value.addListener(this);
    smoothed_value.reset(refresh_rate, smoothing_time);
}

CompressorMeterComponent::~CompressorMeterComponent()
{
}

void CompressorMeterComponent::timerCallback()
{
    if (raw_value > smoothed_value.getCurrentValue())
    {
        smoothed_value.setCurrentAndTargetValue(raw_value);
    }
    else
    {
        smoothed_value.setTargetValue(raw_value);
        smoothed_value.getNextValue();
    }
    gain_reduction_slider.setValue(
        smoothed_value.getCurrentValue(), juce::dontSendNotification
    );
    // repaint();
}

void CompressorMeterComponent::visibilityChanged()
{
    juce::MessageManager::callAsync(
        [this]
        {
            if (isShowing())
                startTimerHz(refresh_rate);
            else
            {
                stopTimer();
            }
        }
    );
}

void CompressorMeterComponent::valueChanged(juce::Value& v)
{
    raw_value = -1.0f * static_cast<float>(v.getValue());
}

void CompressorMeterComponent::paint(juce::Graphics& g)
{
    ignoreUnused(g);
}

void CompressorMeterComponent::resized()
{
    auto bounds = getLocalBounds();
    gain_reduction_slider.setBounds(bounds.withSizeKeepingCentre(
        CompressorDimensions::GAIN_REDUCTION_WIDTH,
        CompressorDimensions::GAIN_REDUCTION_HEIGHT
    ));
}

void CompressorMeterComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    gain_reduction_slider.setColour(
        juce::Slider::rotarySliderFillColourId, colour1
    );
    repaint();
}
