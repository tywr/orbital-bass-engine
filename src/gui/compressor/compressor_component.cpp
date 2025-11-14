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

    paintMeter(g, colour1, colour2);
}

void CompressorComponent::paintMeter(
    juce::Graphics& g, juce::Colour colour1, juce::Colour colour2
)
{
    // Draw meter border
    float meter_border_thickness = CompressorDimensions::METER_BORDER_THICKNESS;
    float border_radius = CompressorDimensions::BORDER_RADIUS;

    auto bounds = getLocalBounds().withSizeKeepingCentre(
        CompressorDimensions::WIDTH, CompressorDimensions::HEIGHT
    );
    bounds.removeFromRight(CompressorDimensions::SIDE_WIDTH / 2);
    auto meter_bounds =
        bounds.removeFromRight(CompressorDimensions::GAIN_REDUCTION_WIDTH)
            .withSizeKeepingCentre(
                CompressorDimensions::GAIN_REDUCTION_WIDTH,
                CompressorDimensions::GAIN_REDUCTION_HEIGHT
            );
    auto outer_meter_bounds =
        meter_bounds
            .expanded((int)meter_border_thickness, (int)meter_border_thickness)
            .toFloat();
    juce::Path meter_border_path;
    meter_border_path.addRoundedRectangle(
        outer_meter_bounds, border_radius + meter_border_thickness
    );
    meter_border_path.addRoundedRectangle(
        outer_meter_bounds.reduced(
            meter_border_thickness, meter_border_thickness
        ),
        border_radius
    );
    meter_border_path.setUsingNonZeroWinding(false);

    juce::ColourGradient meter_border_gradient(
        colour1, outer_meter_bounds.getTopLeft(), colour2,
        outer_meter_bounds.getBottomLeft(), false
    );
    g.setGradientFill(meter_border_gradient);
    g.fillPath(meter_border_path);

    // Draw meter background
    float height = meter_bounds.getHeight();
    float width = meter_bounds.getWidth();
    float offset = CompressorDimensions::METER_OFFSET_Y * (float)height;
    float x_anchor = (float)meter_bounds.getX() + 0.5f * (float)width;
    float y_anchor = (float)(meter_bounds.getY() + height + offset);
    float length =
        CompressorDimensions::METER_POINTER_LENGTH * (float)height + offset;
    float marker_length = CompressorDimensions::METER_MARKER_LENGTH;

    g.setColour(ColourCodes::grey0);
    for (int i = 0; i < 6; ++i)
    {
        float alpha_degrees =
            CompressorDimensions::METER_START_ANGLE +
            (float)i / 5.0f * CompressorDimensions::METER_ANGLE_RANGE;
        float alpha = alpha_degrees * juce::MathConstants<float>::pi / 180.0f;
        float x_end = x_anchor + (length)*std::cos(alpha);
        float x_start = x_anchor + (length - marker_length) * std::cos(alpha);
        float y_end = y_anchor + (length)*std::sin(alpha);
        float y_start = y_anchor + (length - marker_length) * std::sin(alpha);
        juce::Path p;
        p.startNewSubPath(x_start, y_start);
        p.lineTo(x_end, y_end);
        g.strokePath(
            p, juce::PathStrokeType(
                   2.0f, juce::PathStrokeType::JointStyle::curved,
                   juce::PathStrokeType::EndCapStyle::rounded
               )
        );

        juce::String textToDraw = juce::String(i * 4);

        float textPadding = 15.0f; // 10 pixels padding from the outer tick
        float textRadius = length - marker_length - textPadding;
        float x_text_centre = x_anchor + textRadius * std::cos(alpha);
        float y_text_centre = y_anchor + textRadius * std::sin(alpha);

        float textBoxWidth = 25.0f;
        float textBoxHeight = 15.0f;
        juce::Rectangle<float> textBox(textBoxWidth, textBoxHeight);
        textBox.setCentre(x_text_centre, y_text_centre);

        g.drawFittedText(
            textToDraw, textBox.toNearestInt(), juce::Justification::centred, 1
        );
    }
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
