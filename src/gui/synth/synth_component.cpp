#include "synth_component.h"
#include "../colours.h"
#include "synth_dimensions.h"
#include "synth_voice_knobs_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

SynthComponent::SynthComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), voice_knobs_component(params)
{
    addAndMakeVisible(voice_knobs_component);
    addAndMakeVisible(bypass_button);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "synth_bypass", bypass_button
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

SynthComponent::~SynthComponent()
{
}

void SynthComponent::paint(juce::Graphics& g)
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
    float border_thickness = SynthDimensions::BORDER_THICKNESS;
    float border_radius = SynthDimensions::CORNER_RADIUS;

    auto outer_bounds = getLocalBounds()
                            .withSizeKeepingCentre(
                                SynthDimensions::WIDTH, SynthDimensions::HEIGHT
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

    voice_knobs_component.switchColour(colour1, colour2);
}

void SynthComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        SynthDimensions::WIDTH, SynthDimensions::HEIGHT
    );
    auto middle_bounds = bounds.withSizeKeepingCentre(
        bounds.getWidth() - SynthDimensions::SIDE_WIDTH * 2,
        SynthDimensions::BOX_HEIGHT
    );
    voice_knobs_component.setBounds(middle_bounds);

    auto left_bounds = bounds.removeFromLeft(SynthDimensions::SIDE_WIDTH);
    bypass_button.setBounds(left_bounds.withSizeKeepingCentre(
        SynthDimensions::BYPASS_SIZE, SynthDimensions::BYPASS_SIZE
    ));
}
