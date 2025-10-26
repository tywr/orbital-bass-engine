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
    bounds.removeFromBottom(CompressorDimensions::FOOTER_HEIGHT);
    auto meter_bounds =
        bounds.removeFromBottom(CompressorDimensions::GAIN_REDUCTION_HEIGHT)
            .withSizeKeepingCentre(
                CompressorDimensions::GAIN_REDUCTION_WIDTH,
                CompressorDimensions::GAIN_REDUCTION_HEIGHT
            );
    g.setColour(ColourCodes::bg1);
    g.fillRect(meter_bounds);
}

void CompressorComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        CompressorDimensions::WIDTH, CompressorDimensions::HEIGHT
    );
    bypass_button.setBounds(
        bounds.removeFromBottom(CompressorDimensions::FOOTER_HEIGHT)
            .withSizeKeepingCentre(
                CompressorDimensions::BYPASS_BUTTON_WIDTH,
                CompressorDimensions::BYPASS_BUTTON_HEIGHT
            )
    );
    meter_component.setBounds(
        bounds.removeFromBottom(CompressorDimensions::GAIN_REDUCTION_HEIGHT)
    );
    title_label.setBounds(
        bounds.removeFromBottom(CompressorDimensions::TITLE_LABEL_HEIGHT)
    );
    bounds.removeFromTop(CompressorDimensions::INNER_Y_TOP_PADDING);
    knobs_component.setBounds(bounds.removeFromTop(
        CompressorDimensions::KNOBS_TOP_BOX_HEIGHT +
        CompressorDimensions::KNOBS_BOTTOM_BOX_HEIGHT +
        CompressorDimensions::KNOBS_ROW_PADDING
    ));
}
