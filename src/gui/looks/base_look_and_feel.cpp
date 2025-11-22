#include "base_look_and_feel.h"

#include "../colours.h"
#include <juce_gui_basics/juce_gui_basics.h>

BaseLookAndFeel::BaseLookAndFeel()
{
    setColourScheme(getColourScheme());
}

void BaseLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button,
    const juce::Colour& backgroundColour, bool isMouseOverButton,
    bool isButtonDown
)
{
    auto bounds = button.getLocalBounds().toFloat();

    // Get appropriate colour based on button state
    juce::Colour colour = backgroundColour;

    // For toggle-style behavior, check if it's a TextButton with toggle state
    auto* textButton = dynamic_cast<juce::TextButton*>(&button);
    if (textButton && textButton->getClickingTogglesState() &&
        textButton->getToggleState())
    {
        colour = button.findColour(juce::TextButton::buttonOnColourId);
    }
    else
    {
        colour = button.findColour(juce::TextButton::buttonColourId);
    }

    // Apply interaction states
    if (isButtonDown)
        colour = colour.darker(0.2f);
    else if (isMouseOverButton)
        colour = colour.brighter(0.2f);

    g.setColour(colour);
    g.drawEllipse(bounds.reduced(strokeWidth), strokeWidth);
}

void BaseLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown
)
{
    auto bounds = button.getLocalBounds();
    auto text = button.getButtonText();
    juce::Colour colour;
    if (button.getClickingTogglesState() && button.getToggleState())
    {
        colour = button.findColour(juce::TextButton::textColourOnId);
    }
    else
    {
        colour = button.findColour(juce::TextButton::textColourOffId);
    }

    if (shouldDrawButtonAsHighlighted)
    {
        colour = colour.brighter(0.2f);
    }
    else if (shouldDrawButtonAsDown)
    {
        colour = colour.darker(0.2f);
    }
    g.setFont(mainFont);
    g.setColour(colour);
    g.drawFittedText(text, bounds, juce::Justification::centred, 1);
}

void BaseLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
    bool isButtonDown
)
{
    auto bounds = button.getLocalBounds().toFloat();

    // Define switch dimensions (vertical pill)
    const float switchWidth = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const float switchHeight = switchWidth * 2.0f;
    const float thumbSize = switchWidth * 0.8f;
    const float padding = (switchWidth - thumbSize) / 2.0f;

    // Center the switch in the button bounds
    auto switchBounds = juce::Rectangle<float>(switchWidth, switchHeight)
        .withCentre(bounds.getCentre());

    // Determine colors based on state
    juce::Colour trackColour;
    if (button.getToggleState())
        trackColour = button.findColour(juce::ToggleButton::tickColourId);
    else
        trackColour = button.findColour(juce::ToggleButton::tickDisabledColourId);

    if (isMouseOverButton)
        trackColour = trackColour.brighter(0.1f);

    // Draw the pill-shaped track
    g.setColour(trackColour);
    g.fillRoundedRectangle(switchBounds, switchWidth / 2.0f);

    // Calculate thumb position (bottom when on, top when off)
    float thumbY;
    if (button.getToggleState())
        thumbY = switchBounds.getBottom() - thumbSize - padding; // Bottom position (ON)
    else
        thumbY = switchBounds.getY() + padding; // Top position (OFF)

    float thumbX = switchBounds.getCentreX() - thumbSize / 2.0f;

    // Draw the circular thumb
    juce::Colour thumbColour = juce::Colours::white;
    if (isButtonDown)
        thumbColour = thumbColour.darker(0.1f);

    g.setColour(thumbColour);
    g.fillEllipse(thumbX, thumbY, thumbSize, thumbSize);
}

void BaseLookAndFeel::drawRotarySlider(
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
    // draw background
    g.setColour(juce::Colours::black);
    g.fillEllipse(
        bounds.getCentreX() - (radius - lineW) + strokeWidth / 2.0f,
        bounds.getCentreY() - (radius - lineW) + strokeWidth / 2.0f,
        (radius - lineW) * 2 - strokeWidth, (radius - lineW) * 2 - strokeWidth
    );
    // draw marker
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

void BaseLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider
)
{
    juce::ignoreUnused(minSliderPos, maxSliderPos);
    g.fillAll(ColourCodes::grey0);

    juce::Rectangle<float> filledTrack;
    if (style == juce::Slider::LinearBar)
    {
        filledTrack = juce::Rectangle<float>(x, y, sliderPos, height);
    }
    else if (style == juce::Slider::LinearBarVertical)
    {
        // revert to always display from bottom to top
        auto deltaY = sliderPos - y;
        filledTrack = juce::Rectangle<float>(x, y + deltaY, width, height);
    }
    g.setColour(slider.findColour(juce::Slider::trackColourId));
    g.fillRect(filledTrack);
}

void BaseLookAndFeel::drawTabbedButtonBarBackground(
    juce::TabbedButtonBar& buttonBar, juce::Graphics& g
)
{
    juce::ignoreUnused(g, buttonBar);
}

void BaseLookAndFeel::drawTabButton(
    juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver,
    bool isMouseDown
)
{
    const juce::String text = button.getButtonText();
    const juce::Rectangle<int> bounds = button.getLocalBounds().reduced(10);
    const bool isActive = button.isFrontTab();

    // g.fillRect(bounds);

    juce::Colour colour;
    if (isActive)
    {
        colour = ColourCodes::white0;
    }
    else
    {
        colour = ColourCodes::grey2;
    }
    if (isMouseOver)
    {
        colour = colour.brighter(0.2f);
    }
    else if (isMouseDown)
    {
        colour = colour.darker(0.2f);
    }
    g.setFont(mainFont);
    g.setColour(colour);
    g.drawFittedText(text, bounds, juce::Justification::centred, 1);
}

void BaseLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    auto bounds = label.getLocalBounds();
    juce::String text = label.getText();
    g.setFont(mainFont);
    g.setColour(label.findColour(juce::Label::textColourId));
    g.drawFittedText(text, bounds, juce::Justification::centred, 1);
}
