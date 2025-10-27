#include "compressor_look_and_feel.h"
#include "../compressor/compressor_dimensions.h"
#include <cmath>

#include "../colours.h"
#include <juce_gui_basics/juce_gui_basics.h>

void CompressorLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
)
{
    const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();

    auto radius = fmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    const auto toAngle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = fmin(strokeWidth, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(
        bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
        rotaryStartAngle, rotaryEndAngle, true
    );

    g.setColour(ColourCodes::grey0);
    g.strokePath(
        backgroundArc,
        juce::PathStrokeType(
            lineW, juce::PathStrokeType::mitered, juce::PathStrokeType::square
        )
    );

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(
            bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
            0.0f, rotaryStartAngle, toAngle, true
        );

        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(
            valueArc, juce::PathStrokeType(
                          lineW, juce::PathStrokeType::mitered,
                          juce::PathStrokeType::square
                      )
        );
    }
    const float markerLength = radius * 0.2f;
    const float markerThickness = lineW * 0.5f;
    const auto centre = bounds.getCentre();
    juce::Point<float> markerStart =
        centre.getPointOnCircumference(arcRadius - 2 * lineW, toAngle);
    juce::Point<float> markerEnd = centre.getPointOnCircumference(
        arcRadius - markerLength - 2 * lineW, toAngle
    );
    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    g.drawLine(
        markerStart.getX(), markerStart.getY(), markerEnd.getX(),
        markerEnd.getY(), markerThickness
    );
}

void CompressorLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    auto bounds = label.getLocalBounds();
    juce::String text = label.getText();
    g.setFont(main_font);
    g.setColour(label.findColour(juce::Label::textColourId));
    g.drawFittedText(text, bounds, juce::Justification::centred, 1);
}

void CompressorLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider

)
{
    juce::ignoreUnused(width, minSliderPos, maxSliderPos, style, slider);
    float offset = CompressorDimensions::METER_OFFSET_Y * (float)height;
    float ratio = (float)(sliderPos - x) / width;
    float alpha_degrees = CompressorDimensions::METER_START_ANGLE +
                          CompressorDimensions::METER_ANGLE_RANGE * ratio;
    float alpha = alpha_degrees * juce::MathConstants<float>::pi / 180.0f;

    float length =
        CompressorDimensions::METER_POINTER_LENGTH * (float)height + offset;
    float x_anchor = (float)x + (float)width * 0.5f;
    float y_anchor = (float)(y + height + offset);
    float x_end = x_anchor + length * std::cos(alpha);
    float y_end = y_anchor + length * std::sin(alpha);

    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
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
