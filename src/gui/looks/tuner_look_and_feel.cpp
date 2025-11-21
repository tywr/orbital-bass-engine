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
    juce::Colour colour;
    if (button.getToggleState())
        colour =
            button.findColour(juce::ToggleButton::tickColourId); // "On" colour
    else
        colour = button.findColour(
            juce::ToggleButton::tickDisabledColourId
        ); // "Off" colour

    if (isButtonDown)
        colour = button.findColour(juce::ToggleButton::tickDisabledColourId);
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);

    const float buttonStrokeWidth = 2.0f;
    auto bounds =
        button.getLocalBounds().toFloat().reduced(buttonStrokeWidth / 2.0f);

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto area = bounds.withSizeKeepingCentre(size, size);

    auto centerX = area.getCentreX();
    auto centerY = area.getCentreY();

    // Tuning fork shape
    juce::Path tuningFork;

    float forkWidth = size * 0.4f;
    float forkHeight = size * 0.5f;

    float leftProngX = centerX - forkWidth / 2.0f;
    float rightProngX = centerX + forkWidth / 2.0f;
    float topY = area.getY() + size * 0.1f;
    float curveY = topY + forkHeight;
    float bottomY = area.getBottom() - size * 0.05f;

    // Left prong
    tuningFork.startNewSubPath(leftProngX, topY);
    tuningFork.lineTo(leftProngX, curveY);

    // Curve at bottom connecting to handle
    tuningFork.quadraticTo(leftProngX, curveY + forkHeight * 0.3f, centerX, curveY + forkHeight * 0.3f);

    // Handle
    tuningFork.lineTo(centerX, bottomY);

    // Back up to curve
    tuningFork.startNewSubPath(centerX, curveY + forkHeight * 0.3f);
    tuningFork.quadraticTo(rightProngX, curveY + forkHeight * 0.3f, rightProngX, curveY);

    // Right prong
    tuningFork.lineTo(rightProngX, topY);

    g.strokePath(
        tuningFork, juce::PathStrokeType(
                        buttonStrokeWidth, juce::PathStrokeType::curved,
                        juce::PathStrokeType::rounded
                    )
    );
}
