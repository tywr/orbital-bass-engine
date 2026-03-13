#include "tuner_look_and_feel.h"

#include <juce_gui_basics/juce_gui_basics.h>

TunerLookAndFeel::TunerLookAndFeel()
{
    setColourScheme(getColourScheme());
}

void TunerLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
    bool isButtonDown
)
{
    juce::Colour colour = ColourCodes::white0;

    if (isButtonDown)
        colour = ColourCodes::grey3;
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);

    auto bounds = button.getLocalBounds().toFloat();
    float iconSize = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto iconBounds = bounds.withSizeKeepingCentre(iconSize, iconSize);
    float centerX = iconBounds.getCentreX();
    float centerY = iconBounds.getCentreY();

    float stroke = 1.4f;
    juce::PathStrokeType strokeType(stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

    float forkWidth = iconSize * 0.5f;
    float forkHeight = iconSize * 0.5f;
    float handleLength = iconSize * 0.35f;
    float dotRadius = 1.8f;
    float dotGap = stroke + dotRadius;

    // Total height from prong tops to dot bottom, used to vertically center the whole shape
    float totalHeight = forkHeight + handleLength + dotGap + dotRadius;
    float offsetY = centerY - totalHeight / 2.0f;

    float leftProngX = centerX - forkWidth / 2.0f;
    float rightProngX = centerX + forkWidth / 2.0f;
    float topY = offsetY;
    float curveY = topY + forkHeight;
    float handleEndY = curveY + handleLength;

    juce::Path tuningFork;

    tuningFork.startNewSubPath(leftProngX, topY);
    tuningFork.lineTo(leftProngX, curveY);
    tuningFork.quadraticTo(leftProngX, curveY + handleLength * 0.25f, centerX, curveY + handleLength * 0.25f);
    tuningFork.lineTo(centerX, handleEndY);

    tuningFork.startNewSubPath(centerX, curveY + handleLength * 0.25f);
    tuningFork.quadraticTo(rightProngX, curveY + handleLength * 0.25f, rightProngX, curveY);
    tuningFork.lineTo(rightProngX, topY);

    g.strokePath(tuningFork, strokeType);

    float dotY = handleEndY + dotGap;
    g.fillEllipse(centerX - dotRadius, dotY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
}
