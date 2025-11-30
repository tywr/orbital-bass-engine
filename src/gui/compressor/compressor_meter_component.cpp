#include "compressor_meter_component.h"
#include "compressor_dimensions.h"

CompressorMeterComponent::CompressorMeterComponent(juce::Value& v)
    : gain_reduction_value(v)
{
    startTimerHz(refresh_rate);
    gain_reduction_value.addListener(this);
    smoothed_value.reset(refresh_rate, smoothing_time);
}

CompressorMeterComponent::~CompressorMeterComponent()
{
}

void CompressorMeterComponent::timerCallback()
{
    // Use the peak value captured since last timer callback
    if (peak_value > smoothed_value.getCurrentValue())
    {
        smoothed_value.setCurrentAndTargetValue(peak_value);
    }
    else
    {
        smoothed_value.setTargetValue(peak_value);
        smoothed_value.getNextValue();
    }
    // Reset peak for next frame
    peak_value = 0.0f;
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
    // Track the peak (maximum) value
    if (raw_value > peak_value)
    {
        peak_value = raw_value;
    }
}

void CompressorMeterComponent::paint(juce::Graphics& g)
{
    // ignoreUnused(g);
    float v = smoothed_value.getCurrentValue();
    float max_db = 20.0f;

    float width = getWidth();
    float height = getHeight();
    float offset = CompressorDimensions::METER_OFFSET_Y * (float)height;
    float ratio = juce::jlimit(0.0f, 1.0f, (float)(v / max_db));
    float alpha_degrees = CompressorDimensions::METER_START_ANGLE +
                          CompressorDimensions::METER_ANGLE_RANGE * ratio;
    float alpha = alpha_degrees * 3.14159265359f / 180.0f;

    float length =
        CompressorDimensions::METER_POINTER_LENGTH * (float)height + offset;
    float x_anchor = (float)width * 0.5f;
    float y_anchor = (float)(height + offset);
    float x_end = x_anchor + length * std::cos(alpha);
    float y_end = y_anchor + length * std::sin(alpha);

    g.setColour(colour);
    juce::Path p;
    p.startNewSubPath(x_anchor, y_anchor);
    p.lineTo(x_end, y_end);
    g.strokePath(
        p, juce::PathStrokeType(
               2.0f, juce::PathStrokeType::JointStyle::curved,
               juce::PathStrokeType::EndCapStyle::rounded
           )
    );
}

void CompressorMeterComponent::resized()
{
    auto bounds = getLocalBounds();
}

void CompressorMeterComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    colour = colour1;
    repaint();
}
