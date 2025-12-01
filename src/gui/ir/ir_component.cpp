#include "ir_component.h"
#include "../colours.h"
#include "ir_dimensions.h"
#include "../dimensions.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

IRComponent::IRComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
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
        juce::Font(juce::FontOptions("Fixedsys Core", 24.0f, juce::Font::plain)), true
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
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.slider->setTextBoxStyle(
            juce::Slider::NoTextBox, false, IRDimensions::KNOB_SIZE,
            IRDimensions::KNOB_SIZE
        );
        knob.slider->setColour(
            juce::Slider::rotarySliderFillColourId, ColourCodes::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, *knob.slider
            )
        );
        setupSliderTooltipHandling(knob.slider, knob.label);
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
        colour1 = ColourCodes::white0;
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
    auto title_bounds = full_bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    g.setColour(ColourCodes::bg2);
    g.fillRect(title_bounds);
    g.setColour(border_colour);
    g.drawRect(title_bounds, border_thickness);

    // Draw display section border (upper half)
    auto display_section_bounds = full_bounds.removeFromTop(full_bounds.getHeight() / 2);
    g.setColour(border_colour);
    g.drawRect(display_section_bounds, border_thickness);

    // Draw knobs section border (lower half)
    g.setColour(border_colour);
    g.drawRect(full_bounds, border_thickness);

    // Keep original display_bounds calculation unchanged
    auto bounds =
        getLocalBounds()
            .withSizeKeepingCentre(IRDimensions::WIDTH, IRDimensions::HEIGHT)
            .toFloat();
    auto middle_bounds = bounds.withSizeKeepingCentre(
        IRDimensions::WIDTH -
            2 * (IRDimensions::SIDE_PADDING + IRDimensions::SIDE_WIDTH),
        IRDimensions::HEIGHT
    );

    auto display_bounds =
        middle_bounds.removeFromRight(IRDimensions::IR_LABEL_WIDTH)
            .withSizeKeepingCentre(
                IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
            );
    type_display.setBoundingBox(display_bounds);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(display_bounds.toFloat(), 5.0f);
    type_display.draw(g, 1.0f);
}

void IRComponent::resized()
{
    auto full_bounds = getLocalBounds();

    // Title bar with label and bypass button
    auto title_bounds = full_bounds.removeFromTop(GuiDimensions::PANEL_TITLE_BAR_HEIGHT);
    title_label.setBounds(title_bounds.removeFromLeft(100.0f));
    bypassButton.setBounds(
        title_bounds.removeFromRight(GuiDimensions::BYPASS_BUTTON_WIDTH)
    );

    // Keep original bounds calculation for display and knobs
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        IRDimensions::WIDTH, IRDimensions::HEIGHT
    );
    auto middle_bounds = bounds.withSizeKeepingCentre(
        IRDimensions::WIDTH -
            2 * (IRDimensions::SIDE_PADDING + IRDimensions::SIDE_WIDTH),
        IRDimensions::HEIGHT
    );

    auto display_bounds =
        middle_bounds.removeFromRight(IRDimensions::IR_LABEL_WIDTH)
            .withSizeKeepingCentre(
                IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
            );
    type_display.setBounds(display_bounds.withSizeKeepingCentre(
        IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
    ));

    auto knob_bounds = middle_bounds.withSizeKeepingCentre(
        middle_bounds.getWidth(), IRDimensions::BOX_HEIGHT
    );
    auto label_bounds = knob_bounds.removeFromTop(IRDimensions::LABEL_HEIGHT);

    const int knob_box_size = knob_bounds.getWidth() / (int)knobs.size();
    for (size_t i = 0; i < 3; ++i)
    {
        IRKnob knob = knobs[i];
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      knob_box_size, IRDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(knob_bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       IRDimensions::KNOB_SIZE,
                                       IRDimensions::KNOB_SIZE
                                   ));
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
        current_colour = ColourCodes::white0;
    }
    for (auto knob : knobs)
    {
        knob.slider->setColour(
            juce::Slider::rotarySliderFillColourId, current_colour
        );
    }
    type_display.setColour(current_colour);
    repaint();
}

void IRComponent::setupSliderTooltipHandling(
    juce::Slider* slider, juce::Label* label
)
{

    slider->onDragStart = [this, sl = slider, lab = label]()
    {
        slider_being_dragged = true;
        drag_tooltip.setVisible(false);
        // delay using a Timer
        juce::Timer::callAfterDelay(
            300,
            [this, sl, lab]()
            {
                if (sl->isMouseButtonDown())
                {
                    drag_tooltip.setText(
                        juce::String(sl->getValue(), 2),
                        juce::dontSendNotification
                    );
                    drag_tooltip.setBounds(
                        lab->getX(), lab->getY(), lab->getWidth(),
                        lab->getHeight()
                    );
                    drag_tooltip.toFront(true);
                    drag_tooltip.setVisible(true);
                    drag_tooltip.repaint();
                }
            }
        );
    };
    slider->onDragEnd = [this]()
    {
        slider_being_dragged = false;
        drag_tooltip.setVisible(false);
    };
    slider->onValueChange = [this, sl = slider]()
    {
        if (slider_being_dragged && drag_tooltip.isVisible())
            drag_tooltip.setText(
                juce::String(sl->getValue(), 2), juce::dontSendNotification
            );
    };
}
