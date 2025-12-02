#include "ir_component.h"
#include "../colours.h"
#include "../dimensions.h"
#include "ir_dimensions.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

IRComponent::IRComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
    addAndMakeVisible(title_label);
    title_label.setText("IMPULSE", juce::dontSendNotification);
    title_label.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(drag_tooltip);
    drag_tooltip.setJustificationType(juce::Justification::centred);
    drag_tooltip.setAlwaysOnTop(true);
    drag_tooltip.setColour(juce::Label::backgroundColourId, ColourCodes::bg0);

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("bypass");
    bypassButton.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypassButton.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::white0
    );
    bypassButton.onClick = [this]() { switchColour(); };
    bypassButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "ir_bypass", bypassButton
        );

    addAndMakeVisible(type_display);
    type_display.setFont(
        juce::Font(
            juce::FontOptions("Fixedsys Core", 24.0f, juce::Font::plain)
        ),
        true
    );
    type_display.setJustification(juce::Justification::centred);
    type_display.setColour(ColourCodes::grey3);
    auto* parameter = parameters.getParameter("ir_type");
    type_display_attachment = std::make_unique<juce::ParameterAttachment>(
        *parameter,
        [this, parameter](float new_value)
        {
            juce::StringArray values = parameter->getAllValueStrings();
            juce::String text = values[(int)new_value];
            type_display.setText(text);
            repaint();
        }
    );
    type_display_attachment->sendInitialUpdate();

    for (auto knob : knobs)
    {
        addAndMakeVisible(knob.knob);
        knob.knob->setLabelText(knob.label_text);
        knob.knob->setKnobSize(
            IRDimensions::KNOB_SIZE, IRDimensions::KNOB_SIZE
        );
        knob.knob->setLabelHeight(IRDimensions::LABEL_HEIGHT);
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderOutlineColourId, ColourCodes::grey3
        );
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, knob.knob->getSlider()
            )
        );
        setupSliderTooltipHandling(knob.knob);
    }
    switchColour();
}

IRComponent::~IRComponent()
{
}

void IRComponent::paint(juce::Graphics& g)
{
    bool bypass = bypassButton.getToggleState();
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
    auto full_bounds = getLocalBounds();

    title_label.setColour(juce::Label::textColourId, colour2);

    // Fill background
    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRect(full_bounds);

    // Draw outer border
    g.setColour(border_colour);
    g.drawRect(full_bounds, border_thickness);

    // Draw title bar background and border
    auto title_bounds =
        full_bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    g.setColour(ColourCodes::bg2);
    g.fillRect(title_bounds);
    g.setColour(border_colour);
    g.drawRect(title_bounds, border_thickness);

    // Draw display section border (upper half)
    auto display_section_bounds =
        full_bounds.removeFromTop(full_bounds.getHeight() / 2);
    g.setColour(border_colour);
    g.drawRect(display_section_bounds, border_thickness);

    // Draw knobs section border (lower half)
    g.setColour(border_colour);
    g.drawRect(full_bounds, border_thickness);

    // Calculate display bounds to match resized() layout
    auto bounds_for_display = getLocalBounds();
    bounds_for_display.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    auto display_section =
        bounds_for_display.removeFromTop(bounds_for_display.getHeight() / 2);

    auto display_bounds =
        display_section
            .withSizeKeepingCentre(
                IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
            )
            .toFloat();

    type_display.setBoundingBox(display_bounds);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(display_bounds, 5.0f);
    type_display.draw(g, 1.0f);
}

void IRComponent::resized()
{
    auto full_bounds = getLocalBounds();

    // Title bar with label and bypass button
    auto title_bounds =
        full_bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_label.setBounds(title_bounds.removeFromLeft(100.0f));
    bypassButton.setBounds(title_bounds
                               .removeFromRight(
                                   GuiDimensions::BYPASS_BUTTON_WIDTH +
                                   GuiDimensions::BYPASS_BUTTON_PADDING
                               )
                               .reduced(GuiDimensions::PANEL_BORDER_THICKNESS));

    // Split remaining bounds into top row (display) and bottom row (knobs)
    auto display_section =
        full_bounds.removeFromTop(full_bounds.getHeight() / 2);
    auto knobs_section = full_bounds;

    // Position display centered in top section
    auto display_bounds = display_section.withSizeKeepingCentre(
        IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
    );
    type_display.setBounds(display_bounds);

    // Position 3 knobs horizontally in bottom section
    auto knob_area = knobs_section.withSizeKeepingCentre(
        knobs_section.getWidth() * 0.8f, IRDimensions::BOX_HEIGHT
    );

    const int knob_box_size = knob_area.getWidth() / (int)knobs.size();
    for (size_t i = 0; i < 3; ++i)
    {
        IRKnob knob = knobs[i];
        knob.knob->setBounds(knob_area.removeFromLeft(knob_box_size));
    }
}

void IRComponent::switchColour()
{
    if (bypassButton.getToggleState())
    {
        current_colour = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    else
    {
        current_colour = ColourCodes::orange;
    }
    for (auto knob : knobs)
    {
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderOutlineColourId, current_colour
        );
    }
    type_display.setColour(current_colour);
    repaint();
}

void IRComponent::setupSliderTooltipHandling(LabeledKnob* knob)
{
    auto& slider = knob->getSlider();
    auto& label = knob->getLabel();

    slider.onDragStart = [this, &slider, &label, knob]()
    {
        slider_being_dragged = true;
        drag_tooltip.setVisible(false);
        // delay using a Timer
        juce::Timer::callAfterDelay(
            300,
            [this, &slider, &label, knob]()
            {
                if (slider.isMouseButtonDown())
                {
                    drag_tooltip.setText(
                        juce::String(slider.getValue(), 2),
                        juce::dontSendNotification
                    );
                    auto labelBounds = getLocalArea(knob, label.getBounds());
                    drag_tooltip.setBounds(labelBounds);
                    drag_tooltip.toFront(true);
                    drag_tooltip.setVisible(true);
                    drag_tooltip.repaint();
                }
            }
        );
    };
    slider.onDragEnd = [this]()
    {
        slider_being_dragged = false;
        drag_tooltip.setVisible(false);
    };
    slider.onValueChange = [this, &slider]()
    {
        if (slider_being_dragged && drag_tooltip.isVisible())
            drag_tooltip.setText(
                juce::String(slider.getValue(), 2), juce::dontSendNotification
            );
    };
}
