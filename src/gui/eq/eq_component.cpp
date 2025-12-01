#include "eq_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "eq_dimensions.h"
#include "eq_sliders_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

EqComponent::EqComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), sliders_component(params)
{
    addAndMakeVisible(title_label);
    title_label.setText("EQ", juce::dontSendNotification);
    title_label.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(sliders_component);

    addAndMakeVisible(bypass_button);
    bypass_button.setButtonText("bypass");
    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, GuiColours::EQ_ACTIVE_COLOUR_1
    );
    bypass_button.onClick = [this]() { repaint(); };

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "eq_bypass", bypass_button
        );
}

EqComponent::~EqComponent()
{
}

void EqComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1, colour2, border_colour;
    if (!bypass)
    {
        colour1 = GuiColours::EQ_ACTIVE_COLOUR_1;
        colour2 = GuiColours::EQ_ACTIVE_COLOUR_2;
        border_colour = ColourCodes::grey0;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        border_colour = ColourCodes::grey0;
    }

    float border_thickness = GuiDimensions::PANEL_BORDER_THICKNESS;
    auto full_bounds = getLocalBounds();

    title_label.setColour(juce::Label::textColourId, colour2);

    // Fill background
    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRect(full_bounds);

    // Draw outer border
    g.setColour(border_colour);
    g.drawRect(full_bounds, border_thickness);

    // Draw title bar background and border
    auto title_bounds = full_bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    g.setColour(ColourCodes::bg2);
    g.fillRect(title_bounds);
    g.setColour(border_colour);
    g.drawRect(title_bounds, border_thickness);

    // Draw main content area border
    g.setColour(border_colour);
    g.drawRect(full_bounds, border_thickness);

    sliders_component.switchColour(colour1, colour2);
}

void EqComponent::resized()
{
    auto full_bounds = getLocalBounds();

    // Title bar with label and bypass button
    auto title_bounds = full_bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_label.setBounds(title_bounds.removeFromLeft(100.0f));
    bypass_button.setBounds(
        title_bounds
            .removeFromRight(
                GuiDimensions::BYPASS_BUTTON_WIDTH +
                GuiDimensions::BYPASS_BUTTON_PADDING
            )
            .reduced(GuiDimensions::PANEL_BORDER_THICKNESS)
    );

    // Position sliders in remaining space
    sliders_component.setBounds(full_bounds);
}
