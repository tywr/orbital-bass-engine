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

    // Define switch dimensions (horizontal slider)
    const float switchHeight =
        juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const float switchWidth = switchHeight * 2.0f;
    const float thumbSize = switchHeight * 0.8f;
    const float padding = (switchHeight - thumbSize) / 2.0f;

    // Center the switch in the button bounds
    auto switchBounds = juce::Rectangle<float>(switchWidth, switchHeight)
                            .withCentre(bounds.getCentre());

    // Determine colors based on state
    juce::Colour trackColour;
    if (button.getToggleState())
        trackColour = button.findColour(juce::ToggleButton::tickColourId);
    else
        trackColour =
            button.findColour(juce::ToggleButton::tickDisabledColourId);

    if (isMouseOverButton)
        trackColour = trackColour.brighter(0.1f);

    // Draw the squared track
    g.setColour(trackColour);
    g.fillRect(switchBounds);

    // Calculate thumb position (right when on, left when off)
    float thumbX;
    if (button.getToggleState())
        thumbX = switchBounds.getRight() - thumbSize -
                 padding; // Right position (ON)
    else
        thumbX = switchBounds.getX() + padding; // Left position (OFF)

    float thumbY = switchBounds.getCentreY() - thumbSize / 2.0f;

    // Draw the squared thumb
    juce::Colour thumbColour = juce::Colours::white;
    if (isButtonDown)
        thumbColour = thumbColour.darker(0.1f);

    g.setColour(thumbColour);
    g.fillRect(thumbX, thumbY, thumbSize, thumbSize);
}

void BaseLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
)
{
    const auto full_bounds =
        juce::Rectangle<int>(x, y, width, height).toFloat();

    auto radius = fmin(full_bounds.getWidth(), full_bounds.getHeight()) / 2.0f;
    const auto bounds =
        full_bounds.withSizeKeepingCentre(2.0f * radius, 2.0f * radius);
    const auto toAngle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = fmin(strokeWidth, radius * 0.5f);

    // Draw background fill
    g.setColour(juce::Colours::black);
    g.fillEllipse(bounds.reduced(lineW));

    // Draw circle outline (outside edge)
    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    g.drawEllipse(bounds.reduced(lineW / 2.0f), lineW);

    // Draw marker
    const float markerLength = radius * 0.4f;
    const float markerThickness = strokeWidth;
    const auto centre = bounds.getCentre();
    juce::Point<float> markerStart =
        centre.getPointOnCircumference(radius - strokeWidth / 2, toAngle);
    juce::Point<float> markerEnd = centre.getPointOnCircumference(
        radius - lineW * 1.5f - markerLength, toAngle
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
