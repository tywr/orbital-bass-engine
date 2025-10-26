#include "chorus_component.h"
#include "../colours.h"
#include "chorus_dimensions.h"
#include "chorus_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

ChorusComponent::ChorusComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), knobs_component(params)
{
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(bypass_button);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "chorus_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId,
        GuiColours::CHORUS_ACTIVE_COLOUR_1
    );
    bypass_button.onClick = [this]() { repaint(); };
}

ChorusComponent::~ChorusComponent()
{
}

void ChorusComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1;
    juce::Colour colour2;
    if (!bypass)
    {
        colour1 = GuiColours::CHORUS_ACTIVE_COLOUR_1;
        colour2 = GuiColours::CHORUS_ACTIVE_COLOUR_2;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    float border_thickness = ChorusDimensions::BORDER_THICKNESS;
    float border_radius = ChorusDimensions::CORNER_RADIUS;

    auto outer_bounds =
        getLocalBounds()
            .withSizeKeepingCentre(
                ChorusDimensions::WIDTH, ChorusDimensions::HEIGHT
            )
            .toFloat();
    auto inner_bounds = outer_bounds.reduced(border_thickness).toFloat();

    g.setColour(GuiColours::CHORUS_BG_COLOUR);
    g.fillRoundedRectangle(inner_bounds, border_radius);

    juce::Path border_path;
    border_path.addRoundedRectangle(
        outer_bounds, border_radius + border_thickness
    );
    border_path.addRoundedRectangle(inner_bounds, border_radius);
    border_path.setUsingNonZeroWinding(false);

    juce::ColourGradient border_gradient(
        colour1, outer_bounds.getTopLeft(), colour2,
        outer_bounds.getBottomLeft(), false
    );
    g.setGradientFill(border_gradient);
    g.fillPath(border_path);

    knobs_component.switchColour(colour1, colour2);
}

void ChorusComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        ChorusDimensions::WIDTH, ChorusDimensions::HEIGHT
    );
    auto middle_bounds = bounds.withSizeKeepingCentre(
        bounds.getWidth() - ChorusDimensions::SIDE_WIDTH * 2, ChorusDimensions::BOX_HEIGHT
    );
    knobs_component.setBounds(middle_bounds);

    auto left_bounds = bounds.removeFromLeft(ChorusDimensions::SIDE_WIDTH);
    bypass_button.setBounds(left_bounds.withSizeKeepingCentre(
        ChorusDimensions::BYPASS_SIZE, ChorusDimensions::BYPASS_SIZE
    ));
}
