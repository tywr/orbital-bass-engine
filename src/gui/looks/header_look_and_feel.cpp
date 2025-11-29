#include "header_look_and_feel.h"

#include "../colours.h"
#include <juce_gui_basics/juce_gui_basics.h>

void HeaderLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
)
{
    const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();

    auto radius = fmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    const auto toAngle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = fmin(stroke_width, radius * 0.5f);
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
            lineW / 2.0f, juce::PathStrokeType::mitered, juce::PathStrokeType::square
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

    const float dotRadius = lineW * 0.45f;
    const float dotDiameter = dotRadius * 2.0f;
    const auto centre = bounds.getCentre();
    juce::Point<float> dotPosition = centre.getPointOnCircumference(
        arcRadius - 2 * lineW - dotRadius, toAngle
    );
    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    g.fillEllipse(
        dotPosition.getX() - dotRadius, dotPosition.getY() - dotRadius,
        dotDiameter, dotDiameter
    );
}
