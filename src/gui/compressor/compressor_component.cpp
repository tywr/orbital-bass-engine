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
    addAndMakeVisible(title_label);
    addAndMakeVisible(knobs_component);
    addAndMakeVisible(meter_component);
    addAndMakeVisible(bypass_button);

    title_label.setText("COMPRESSOR", juce::dontSendNotification);
    title_label.setJustificationType(juce::Justification::centredLeft);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "compressor_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::orange
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
    juce::Colour border_colour;
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

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRect(bounds);

    g.setColour(border_colour);
    g.drawRect(bounds, border_thickness);

    auto title_bounds =
        bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);

    g.setColour(ColourCodes::bg2);
    g.fillRect(title_bounds);

    g.setColour(border_colour);
    g.drawRect(title_bounds, border_thickness);

    bounds.removeFromTop(bounds.getHeight() / 2.0f);
    g.drawRect(bounds, border_thickness);

    knobs_component.switchColour(colour1, colour2);
    meter_component.switchColour(colour1, colour2);

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

    // Draw meter background
    float height = meter_bounds.getHeight();
    float width = meter_bounds.getWidth();
    float offset = CompressorDimensions::METER_OFFSET_Y * (float)height;
    float x_anchor = (float)meter_bounds.getX() + 0.5f * (float)width;
    float y_anchor = (float)(meter_bounds.getY() + height + offset);
    float length =
        CompressorDimensions::METER_POINTER_LENGTH * (float)height + offset;
    float marker_length = height * CompressorDimensions::METER_MARKER_LENGTH;

    g.setColour(ColourCodes::grey0);
    for (int i = 0; i < 6; ++i)
    {
        float alpha_degrees =
            CompressorDimensions::METER_START_ANGLE +
            (float)i / 5.0f * CompressorDimensions::METER_ANGLE_RANGE;
        float alpha = alpha_degrees * 3.14159265359f / 180.0f;

        // Only draw tick marks for 0 and 20 (i == 0 and i == 5)
        if (i == 0 || i == 5)
        {
            float x_end = x_anchor + (length)*std::cos(alpha);
            float x_start =
                x_anchor + (length - marker_length) * std::cos(alpha);
            float y_end = y_anchor + (length)*std::sin(alpha);
            float y_start =
                y_anchor + (length - marker_length) * std::sin(alpha);

            juce::Path p;
            p.startNewSubPath(x_start, y_start);
            p.lineTo(x_end, y_end);
            g.strokePath(
                p, juce::PathStrokeType(
                       2.0f, juce::PathStrokeType::JointStyle::curved,
                       juce::PathStrokeType::EndCapStyle::rounded
                   )
            );
        }
        // Draw number labels for middle values only (i == 1, 2, 3, 4)
        else
        {
            juce::String textToDraw = juce::String(i * 4);

            float x_text_centre =
                x_anchor + (length - marker_length / 2.0f) * std::cos(alpha);
            float y_text_centre =
                y_anchor + (length - marker_length / 2.0f) * std::sin(alpha);

            float textBoxWidth = 25.0f;
            float textBoxHeight = marker_length;
            juce::Rectangle<float> textBox(textBoxWidth, textBoxHeight);
            textBox.setCentre(x_text_centre, y_text_centre);

            g.drawFittedText(
                textToDraw, textBox.toNearestInt(),
                juce::Justification::centred, 1
            );
        }
    }

    // Draw two arcs connecting the 0 and 20 tick marks
    float start_angle = (CompressorDimensions::METER_START_ANGLE +
                         CompressorDimensions::METER_ANGLE_RANGE + 90.0f) *
                        3.14159265359f / 180.0f;
    float end_angle = (90.0f + CompressorDimensions::METER_START_ANGLE) *
                      3.14159265359f / 180.0f;

    juce::Path outer_arc;
    outer_arc.addCentredArc(
        x_anchor, y_anchor, length, length, 0.0f, start_angle, end_angle, true
    );
    g.strokePath(
        outer_arc, juce::PathStrokeType(
                       2.0f, juce::PathStrokeType::JointStyle::curved,
                       juce::PathStrokeType::EndCapStyle::rounded
                   )
    );

    // Inner arc (at the start of tick marks)
    juce::Path inner_arc;
    inner_arc.addCentredArc(
        x_anchor, y_anchor, length - marker_length, length - marker_length,
        0.0f, start_angle, end_angle, true
    );
    g.strokePath(
        inner_arc, juce::PathStrokeType(
                       2.0f, juce::PathStrokeType::JointStyle::curved,
                       juce::PathStrokeType::EndCapStyle::rounded
                   )
    );
}

void CompressorComponent::resized()
{
    auto bounds = getLocalBounds();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight() - GuiDimensions::PANEL_TITLE_BAR_HEIGHT;
    auto title_bounds =
        bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_label.setBounds(title_bounds.removeFromLeft(100.0f));
    bypass_button.setBounds(
        title_bounds.removeFromRight(GuiDimensions::BYPASS_BUTTON_WIDTH)
    );
    meter_component.setBounds(bounds.removeFromTop(height / 2));
    knobs_component.setBounds(bounds);
}
