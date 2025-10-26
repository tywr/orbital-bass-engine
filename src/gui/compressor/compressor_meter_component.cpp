#include "compressor_meter_component.h"
#include "../looks/compressor_look_and_feel.h"
#include "compressor_dimensions.h"

CompressorMeterComponent::CompressorMeterComponent(juce::Value& v)
    : gain_reduction_value(v)
{
    setLookAndFeel(new CompressorLookAndFeel());
    startTimerHz(60);
    addAndMakeVisible(gain_reduction_slider);

    gain_reduction_slider.setRange(0, 20.0f, 0.5f);
    gain_reduction_slider.setSkewFactor(1.0);
    gain_reduction_slider.setValue(0.0);
    gain_reduction_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    gain_reduction_value.addListener(this);
    gain_reduction_value.setValue(0);
}

CompressorMeterComponent::~CompressorMeterComponent()
{
}

void CompressorMeterComponent::timerCallback()
{
    float smoothing_factor = 0.3f;
    smoothed_meter_value +=
        (target_meter_value - smoothed_meter_value) * smoothing_factor;
    if (std::abs(target_meter_value - smoothed_meter_value) < 0.1f)
    {
        smoothed_meter_value = target_meter_value;
    }
    gain_reduction_slider.setValue(
        smoothed_meter_value, juce::dontSendNotification
    );
    // repaint();
}

void CompressorMeterComponent::visibilityChanged()
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

void CompressorMeterComponent::valueChanged(juce::Value& v)
{
    float value = -1 * static_cast<float>(v.getValue());
    target_meter_value = value;
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
