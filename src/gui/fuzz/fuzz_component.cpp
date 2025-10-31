#include "fuzz_component.h"
#include "../colours.h"
#include "fuzz_dimensions.h"
#include "fuzz_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

FuzzComponent::FuzzComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), knobs_component(params)
{
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(bypass_button);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "fuzz_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId,
        GuiColours::FUZZ_ACTIVE_COLOUR_1
    );
    bypass_button.onClick = [this]() { repaint(); };
}

FuzzComponent::~FuzzComponent()
{
}

void FuzzComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1;
    juce::Colour colour2;
    if (!bypass)
    {
        colour1 = GuiColours::FUZZ_ACTIVE_COLOUR_1;
        colour2 = GuiColours::FUZZ_ACTIVE_COLOUR_2;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    float border_thickness = FuzzDimensions::BORDER_THICKNESS;
    float border_radius = FuzzDimensions::CORNER_RADIUS;

    auto outer_bounds = getLocalBounds()
                            .withSizeKeepingCentre(
                                FuzzDimensions::WIDTH, FuzzDimensions::HEIGHT
                            )
                            .toFloat();
    auto inner_bounds = outer_bounds.reduced(border_thickness).toFloat();

    g.setColour(GuiColours::FUZZ_BG_COLOUR);
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

void FuzzComponent::resized()
{
    auto bounds = getLocalBounds()
                      .withSizeKeepingCentre(
                          FuzzDimensions::WIDTH, FuzzDimensions::HEIGHT
                      )
                      .reduced(FuzzDimensions::PADDING);
    knobs_component.setBounds(bounds.removeFromTop(FuzzDimensions::BOX_HEIGHT));

    bypass_button.setBounds(bounds.removeFromBottom(FuzzDimensions::BYPASS_SIZE)
                                .withSizeKeepingCentre(
                                    FuzzDimensions::BYPASS_SIZE,
                                    FuzzDimensions::BYPASS_SIZE
                                ));
}
