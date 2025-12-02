#include "chorus_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "chorus_dimensions.h"
#include "chorus_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

ChorusComponent::ChorusComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), knobs_component(params)
{
    addAndMakeVisible(title_label);
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(bypass_button);

    title_label.setText("CHORUS", juce::dontSendNotification);
    title_label.setJustificationType(juce::Justification::centredLeft);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "chorus_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::orange
    );
    bypass_button.onClick = [this]() { repaint(); };
}

ChorusComponent::~ChorusComponent()
{
}

void ChorusComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1, colour2, border_colour;
    if (!bypass)
    {
        colour1 = ColourCodes::orange;
        colour2 = ColourCodes::white0;
        border_colour = ColourCodes::grey0;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = ColourCodes::grey0;
        border_colour = ColourCodes::grey0;
    }

    float border_thickness = GuiDimensions::PANEL_BORDER_THICKNESS;
    auto bounds = getLocalBounds();

    title_label.setColour(juce::Label::textColourId, colour2);

    // Fill background
    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRect(bounds);

    // Draw outer border
    g.setColour(border_colour);
    g.drawRect(bounds, border_thickness);

    // Draw title bar background and border
    auto title_bounds =
        bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    g.setColour(ColourCodes::bg2);
    g.fillRect(title_bounds);
    g.setColour(border_colour);
    g.drawRect(title_bounds, border_thickness);

    // Draw knobs area border
    g.setColour(border_colour);
    g.drawRect(bounds, border_thickness);

    knobs_component.switchColour(colour1, colour2);
}

void ChorusComponent::resized()
{
    auto bounds = getLocalBounds();

    // Title bar with label and bypass button
    auto title_bounds =
        bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_label.setBounds(title_bounds.removeFromLeft(100.0f));
    bypass_button.setBounds(
        title_bounds
            .removeFromRight(
                GuiDimensions::BYPASS_BUTTON_WIDTH +
                GuiDimensions::BYPASS_BUTTON_PADDING
            )
            .reduced(GuiDimensions::PANEL_BORDER_THICKNESS)
    );

    // Knobs area
    knobs_component.setBounds(bounds);
}
