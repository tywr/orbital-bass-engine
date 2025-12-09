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
    juce::Colour colour = ColourCodes::white0; // Always white like other icons

    if (isButtonDown)
        colour = ColourCodes::grey3;
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);

    const float buttonStrokeWidth = 1.3f;
    auto bounds = button.getLocalBounds().toFloat().reduced(10.5f);

    auto centerX = bounds.getCentreX();
    auto centerY = bounds.getCentreY();

    // Tuning fork shape - scaled to match other icons
    juce::Path tuningFork;

    float forkWidth = bounds.getWidth() * 0.3f;
    float forkHeight = bounds.getHeight() * 0.22f;
    float handleLength = bounds.getHeight() * 0.18f;

    float leftProngX = centerX - forkWidth / 2.0f;
    float rightProngX = centerX + forkWidth / 2.0f;
    float topY = centerY - forkHeight - handleLength / 2.0f;
    float curveY = topY + forkHeight;
    float handleEndY = curveY + handleLength;

    // Left prong
    tuningFork.startNewSubPath(leftProngX, topY);
    tuningFork.lineTo(leftProngX, curveY);

    // Curve at bottom connecting to handle
    tuningFork.quadraticTo(leftProngX, curveY + handleLength * 0.2f, centerX, curveY + handleLength * 0.2f);

    // Handle (tail)
    tuningFork.lineTo(centerX, handleEndY);

    // Back up to curve
    tuningFork.startNewSubPath(centerX, curveY + handleLength * 0.2f);
    tuningFork.quadraticTo(rightProngX, curveY + handleLength * 0.2f, rightProngX, curveY);

    // Right prong
    tuningFork.lineTo(rightProngX, topY);

    g.strokePath(
        tuningFork, juce::PathStrokeType(
                        buttonStrokeWidth, juce::PathStrokeType::curved,
                        juce::PathStrokeType::rounded
                    )
    );

    // Small dot at the bottom of the handle
    float dotRadius = 1.4f;
    float dotY = handleEndY + 1.8f;
    g.fillEllipse(centerX - dotRadius, dotY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
}
