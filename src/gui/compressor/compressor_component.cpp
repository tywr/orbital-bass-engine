#include "compressor_component.h"
#include "../colours.h"
#include "compressor_dimensions.h"
#include "compressor_knobs_component.h"
#include "compressor_meter_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

CompressorComponent::CompressorComponent(
    juce::AudioProcessorValueTreeState& params, juce::Value& value
)
    : parameters(params), knobs_component(params), meter_component(value)
{
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(meter_component);
    addAndMakeVisible(bypass_button);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "compressor_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId,
        GuiColours::COMPRESSOR_ACTIVE_COLOUR_1
    );
    bypass_button.onClick = [this]() { repaint(); };
}

CompressorComponent::~CompressorComponent()
{
}

void CompressorComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1;
    juce::Colour colour2;
    if (!bypass)
    {
        colour1 = GuiColours::COMPRESSOR_ACTIVE_COLOUR_1;
        colour2 = GuiColours::COMPRESSOR_ACTIVE_COLOUR_2;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    float border_thickness = CompressorDimensions::BORDER_THICKNESS;
    float meter_border_thickness = CompressorDimensions::METER_BORDER_THICKNESS;
    float border_radius = CompressorDimensions::BORDER_RADIUS;

    auto outer_bounds =
        getLocalBounds()
            .withSizeKeepingCentre(
                CompressorDimensions::WIDTH, CompressorDimensions::HEIGHT
            )
            .toFloat();
    auto inner_bounds = outer_bounds.reduced(border_thickness).toFloat();

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
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
    meter_component.switchColour(colour1, colour2);

    auto bounds = getLocalBounds().withSizeKeepingCentre(
        CompressorDimensions::WIDTH, CompressorDimensions::HEIGHT
    );
    bounds.removeFromRight(CompressorDimensions::SIDE_WIDTH / 2);
    auto meter_bounds =
        bounds.removeFromRight(CompressorDimensions::GAIN_REDUCTION_WIDTH)
            .withSizeKeepingCentre(
                CompressorDimensions::GAIN_REDUCTION_WIDTH,
                CompressorDimensions::GAIN_REDUCTION_HEIGHT
            )
            .expanded((int)meter_border_thickness, (int)meter_border_thickness)
            .toFloat();
    juce::Path meter_border_path;
    meter_border_path.addRoundedRectangle(
        meter_bounds, border_radius + meter_border_thickness
    );
    meter_border_path.addRoundedRectangle(
        meter_bounds.reduced(meter_border_thickness, meter_border_thickness),
        border_radius
    );
    meter_border_path.setUsingNonZeroWinding(false);

    juce::ColourGradient meter_border_gradient(
        colour1, meter_bounds.getTopLeft(), colour2,
        meter_bounds.getBottomLeft(), false
    );
    g.setGradientFill(meter_border_gradient);
    g.fillPath(meter_border_path);
}

void CompressorComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        CompressorDimensions::WIDTH, CompressorDimensions::HEIGHT
    );
    bounds.removeFromRight(CompressorDimensions::SIDE_WIDTH / 2);
    bypass_button.setBounds(
        bounds.removeFromLeft(CompressorDimensions::SIDE_WIDTH)
            .withSizeKeepingCentre(
                CompressorDimensions::BYPASS_SIZE,
                CompressorDimensions::BYPASS_SIZE
            )
    );
    meter_component.setBounds(
        bounds.removeFromRight(CompressorDimensions::GAIN_REDUCTION_WIDTH)
            .withSizeKeepingCentre(
                CompressorDimensions::GAIN_REDUCTION_WIDTH,
                CompressorDimensions::GAIN_REDUCTION_HEIGHT

            )
    );
    knobs_component.setBounds(
        bounds.removeFromLeft(CompressorDimensions::KNOBS_BOX_WIDTH)
            .withSizeKeepingCentre(
                CompressorDimensions::KNOBS_BOX_WIDTH,
                CompressorDimensions::KNOBS_BOX_HEIGHT
            )
    );
}
