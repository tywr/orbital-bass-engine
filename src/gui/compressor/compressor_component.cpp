#include "compressor_component.h"
#include "../colours.h"
#include "../dimensions.h"
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
    if (!bypass)
    {
        colour1 = GuiColours::COMPRESSOR_ACTIVE_COLOUR_1;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    float border_thickness = GuiDimensions::PANEL_BORDER_THICKNESS;
    auto bounds = getLocalBounds();

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRect(bounds);

    g.setColour(colour1);
    g.drawRect(bounds, border_thickness);

    knobs_component.switchColour(colour1, colour1);
    meter_component.switchColour(colour1, colour1);

    paintMeter(g, colour1, colour1);
}

void CompressorComponent::paintMeter(
    juce::Graphics& g, juce::Colour colour1, juce::Colour colour2
)
{
    auto bounds = getLocalBounds();
    auto bounds_height =
        bounds.getHeight() - GuiDimensions::PANEL_TITLE_BAR_HEIGHT;
    bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    auto meter_bounds = bounds.removeFromTop(bounds_height / 2);

    float border_thickness = GuiDimensions::PANEL_BORDER_THICKNESS;
    g.setColour(colour1);
    g.drawRect(meter_bounds, border_thickness);

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
        float alpha = alpha_degrees * 3.14159265359f / 180.0f;
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
    auto bounds = getLocalBounds();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight() - GuiDimensions::PANEL_TITLE_BAR_HEIGHT;
    auto title_bounds =
        bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    bypass_button.setBounds(
        title_bounds.removeFromRight(GuiDimensions::BYPASS_BUTTON_WIDTH)
    );
    meter_component.setBounds(bounds.removeFromTop(height / 2));
    knobs_component.setBounds(bounds);
}
